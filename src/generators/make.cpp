// Copyright 2011 Boris Kogan (boris@thekogans.net)
//
// This file is part of thekogans_make.
//
// thekogans_make is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// thekogans_make is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with thekogans_make. If not, see <http://www.gnu.org/licenses/>.

#include <set>
#include <iostream>
#include <fstream>
#include <locale>
#include "thekogans/util/Types.h"
#include "thekogans/util/ByteSwap.h"
#include "thekogans/util/Directory.h"
#include "thekogans/util/File.h"
#include "thekogans/util/Path.h"
#include "thekogans/util/Buffer.h"
#include "thekogans/make/core/Function.h"
#include "thekogans/make/core/thekogans_make.h"
#include "thekogans/make/core/Project.h"
#include "thekogans/make/core/Utils.h"
#if defined (THEKOGANS_MAKE_CORE_HAVE_CURL)
    #include "thekogans/make/core/Sources.h"
#endif // defined (THEKOGANS_MAKE_CORE_HAVE_CURL)
#include "thekogans/make/generators/make.h"

namespace thekogans {
    namespace make {

        THEKOGANS_MAKE_CORE_IMPLEMENT_GENERATOR (make)

        namespace {
            void DumpFileLists (
                    std::fstream &stream,
                    const std::list<core::thekogans_make::FileList::Ptr> &fileLists,
                    const core::thekogans_make &thekogans_make,
                    std::list<core::thekogans_make::FileList::File::CustomBuild::Ptr> &customBuildList) {
                for (std::list<core::thekogans_make::FileList::Ptr>::const_iterator
                        it = fileLists.begin (),
                        end = fileLists.end (); it != end; ++it) {
                    std::string namePrefix = core::MakePath (thekogans_make.project_root, (*it)->prefix);
                    std::string outputPrefix =
                        core::MakePath (
                            core::MakePath (
                                thekogans_make.project_root,
                                core::GetBuildDirectory (
                                    thekogans_make.generator,
                                    thekogans_make.config,
                                    thekogans_make.type)),
                            (*it)->prefix);
                    for (std::list<core::thekogans_make::FileList::File::Ptr>::const_iterator
                            jt = (*it)->files.begin (),
                            end = (*it)->files.end (); jt != end; ++jt) {
                        std::string name = core::MakePath (namePrefix, (*jt)->name);
                        stream << "\\\n  " << name;
                        if ((*jt)->customBuild.get () != 0) {
                            core::thekogans_make::FileList::File::CustomBuild::Ptr customBuild (
                                new core::thekogans_make::FileList::File::CustomBuild (
                                    (*jt)->customBuild->message,
                                    (*jt)->customBuild->recipe));
                            for (std::size_t k = 0, count = (*jt)->customBuild->outputs.size (); k < count; ++k) {
                                customBuild->outputs.push_back (
                                    core::MakePath (outputPrefix, (*jt)->customBuild->outputs[k]));
                            }
                            customBuild->dependencies.push_back (name);
                            for (std::size_t k = 0, count = (*jt)->customBuild->dependencies.size (); k < count; ++k) {
                                customBuild->dependencies.push_back (
                                    core::MakePath (thekogans_make.project_root, (*jt)->customBuild->dependencies[k]));
                            }
                            customBuildList.push_back (std::move (customBuild));
                        }
                    }
                }
            }

            std::string GetVariable (
                    util::Buffer &buffer,
                    const char delimiters[] = "()") {
                if (buffer.data[buffer.readOffset] != delimiters[0]) {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Invalid path declaration in: %s", buffer.data);
                }
                std::size_t readOffset = ++buffer.readOffset;
                while (buffer.readOffset < buffer.writeOffset &&
                        buffer.data[buffer.readOffset] != delimiters[1]) {
                    ++buffer.readOffset;
                }
                if (buffer.readOffset == buffer.writeOffset) {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Invalid path declaration in: %s", buffer.data);
                }
                return std::string (
                    (const char *)&buffer.data[readOffset],
                    (const char *)&buffer.data[buffer.readOffset++]);
            }
        }

        bool make::Generate (
                const std::string &project_root,
                const std::string &config,
                const std::string &type,
                bool generateDependencies) {
            struct to_build_system_path : public core::Function {
                static Function::UniquePtr Create () {
                    return Function::UniquePtr (new to_build_system_path);
                }

                virtual core::Value Exec (
                        const core::thekogans_make & /*thekogans_make*/,
                        const Parameters &parameters) const {
                    std::string path;
                    for (Parameters::const_iterator
                             it = parameters.begin (),
                             end = parameters.end (); it != end; ++it) {
                        if ((*it).first == "p" || (*it).first == "path") {
                            path = (*it).second;
                        }
                    }
                    return core::Value (path);
                }
            };
            static core::Function::MapInitializer mapInitializer (
                "to_build_system_path",
                to_build_system_path::Create);
            const core::thekogans_make &thekogans_make =
                core::thekogans_make::GetConfig (
                    project_root,
                    THEKOGANS_MAKE_XML,
                    GetName (),
                    config,
                    type);
            if (rootProject) {
                thekogans_make.CheckDependencies ();
            }
            std::string makefileFilePath = ToSystemPath (
                core::MakePath (
                    core::GetBuildRoot (project_root, GetName (), config, type),
                    MAKEFILE));
            bool makefileFilePathExists = util::Path (makefileFilePath).Exists ();
            time_t makefileFilePathLastModifiedDate = 0;
            if (makefileFilePathExists) {
                makefileFilePathLastModifiedDate =
                    util::Directory::Entry (makefileFilePath).lastModifiedDate;
            }
            bool updatedDependency = false;
            if (generateDependencies) {
                if (thekogans_make.project_type == PROJECT_TYPE_PLUGIN) {
                    for (std::list<core::thekogans_make::Dependency::Ptr>::const_iterator
                            it = thekogans_make.plugin_hosts.begin (),
                            end = thekogans_make.plugin_hosts.end (); it != end; ++it) {
                        if ((*it)->GetConfigFile () == THEKOGANS_MAKE_XML) {
                            make dependency (false);
                            updatedDependency =
                                dependency.Generate (
                                    (*it)->GetProjectRoot (),
                                    (*it)->GetConfig (),
                                    (*it)->GetType (),
                                    generateDependencies) ||
                                updatedDependency ||
                                !makefileFilePathExists ||
                                makefileFilePathLastModifiedDate <
                                util::Directory::Entry (
                                    ToSystemPath (
                                        core::MakePath (
                                            core::GetBuildRoot (
                                                (*it)->GetProjectRoot (),
                                                GetName (),
                                                (*it)->GetConfig (),
                                                (*it)->GetType ()),
                                            MAKEFILE))).lastModifiedDate;
                        }
                    }
                }
                for (std::list<core::thekogans_make::Dependency::Ptr>::const_iterator
                        it = thekogans_make.dependencies.begin (),
                        end = thekogans_make.dependencies.end (); it != end; ++it) {
                    if ((*it)->GetConfigFile () == THEKOGANS_MAKE_XML) {
                        make dependency (false);
                        updatedDependency =
                            dependency.Generate (
                                (*it)->GetProjectRoot (),
                                (*it)->GetConfig (),
                                (*it)->GetType (),
                                generateDependencies) ||
                            updatedDependency ||
                            !makefileFilePathExists ||
                            makefileFilePathLastModifiedDate <
                            util::Directory::Entry (
                                ToSystemPath (
                                    core::MakePath (
                                        core::GetBuildRoot (
                                            (*it)->GetProjectRoot (),
                                            GetName (),
                                            (*it)->GetConfig (),
                                            (*it)->GetType ()), MAKEFILE))).lastModifiedDate;
                    }
                }
            }
            std::string thekogans_makeFilePath =
                ToSystemPath (core::MakePath (project_root, THEKOGANS_MAKE_XML));
            if (updatedDependency ||
                    !makefileFilePathExists ||
                #if defined (THEKOGANS_MAKE_CORE_HAVE_CURL)
                    makefileFilePathLastModifiedDate <
                    util::Directory::Entry (
                        ToSystemPath (
                            core::MakePath (core::_TOOLCHAIN_ROOT, SOURCES_XML))).lastModifiedDate ||
                #endif // defined (THEKOGANS_MAKE_CORE_HAVE_CURL)
                    makefileFilePathLastModifiedDate <
                    util::Directory::Entry (thekogans_makeFilePath).lastModifiedDate) {
                std::cout << "Generating " <<
                    core::MakePath (
                        core::GetBuildRoot (project_root, GetName (), config, type),
                        MAKEFILE) <<
                    std::endl;
                std::cout.flush ();
                core::CreateBuildRoot (project_root, GetName (), config, type);
                std::string makefileTemplatePath =
                    ToSystemPath (
                        core::MakePath (
                            core::MakePath (
                                core::_TOOLCHAIN_ROOT,
                                core::COMMON_DIR + "/" + core::RESOURCES_DIR),
                            MAKEFILE));
                util::ReadOnlyFile makefileTemplate (
                    util::HostEndian,
                    makefileTemplatePath);
                // Protect yourself.
                const util::ui32 MAX_MAKEFILE_TEMPLATE_SIZE = 128 * 1024;
                util::ui32 makefileTemplateSize = (util::ui32)makefileTemplate.GetSize ();
                if (makefileTemplateSize > MAX_MAKEFILE_TEMPLATE_SIZE) {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "'%s' is bigger (%u) than expected. (%u)",
                        makefileTemplatePath.c_str (),
                        makefileTemplateSize,
                        MAX_MAKEFILE_TEMPLATE_SIZE);
                }
                util::Buffer buffer (util::HostEndian, makefileTemplateSize);
                if (buffer.AdvanceWriteOffset (
                        makefileTemplate.Read (
                            buffer.GetWritePtr (),
                            makefileTemplateSize)) != makefileTemplateSize) {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Unable to read %u bytes from '%s'.",
                        makefileTemplateSize,
                        makefileTemplatePath.c_str ());
                }
                std::fstream makefileFile (
                    makefileFilePath.c_str (),
                    std::fstream::out | std::fstream::trunc);
                if (makefileFile.is_open ()) {
                    std::list<core::thekogans_make::FileList::File::CustomBuild::Ptr> customBuildList;
                    while (buffer.GetDataAvailableForReading () != 0) {
                        util::i8 ch;
                        buffer >> ch;
                        switch (ch) {
                            case '$': {
                                std::string variable = GetVariable (buffer);
                                if (variable == "organization") {
                                    makefileFile << thekogans_make.organization;
                                }
                                else if (variable == "project") {
                                    makefileFile << thekogans_make.project;
                                }
                                else if (variable == "project_type") {
                                    makefileFile << thekogans_make.project_type;
                                }
                                else if (variable == "major_version") {
                                    makefileFile << thekogans_make.major_version;
                                }
                                else if (variable == "minor_version") {
                                    makefileFile << thekogans_make.minor_version;
                                }
                                else if (variable == "patch_version") {
                                    makefileFile << thekogans_make.patch_version;
                                }
                                else if (variable == "naming_convention") {
                                    makefileFile << thekogans_make.naming_convention;
                                }
                                else if (variable == "config") {
                                    makefileFile << thekogans_make.config;
                                }
                                else if (variable == "type") {
                                    makefileFile << thekogans_make.type;
                                }
                                else if (variable == "generator") {
                                    makefileFile << GetName ();
                                }
                                else if (variable == "goal") {
                                    makefileFile << thekogans_make.GetProjectGoal ();
                                }
                                else if (variable == "dependencies_goals") {
                                    for (std::list<core::thekogans_make::Dependency::Ptr>::const_iterator
                                            it = thekogans_make.dependencies.begin (),
                                            end = thekogans_make.dependencies.end (); it != end; ++it) {
                                        if ((*it)->GetConfigFile () == THEKOGANS_MAKE_XML) {
                                            makefileFile << "\\\n  " <<
                                                core::thekogans_make::GetConfig (
                                                    (*it)->GetProjectRoot (),
                                                    (*it)->GetConfigFile (),
                                                    (*it)->GetGenerator (),
                                                    (*it)->GetConfig (),
                                                    (*it)->GetType ()).GetProjectGoal ();
                                        }
                                    }
                                }
                                else if (variable == "include_directories") {
                                    std::set<std::string> include_directories;
                                    thekogans_make.GetIncludeDirectories (include_directories);
                                    for (std::set<std::string>::const_iterator
                                            it = include_directories.begin (),
                                            end = include_directories.end (); it != end; ++it) {
                                        makefileFile << "\\\n  " << *it;
                                    }
                                }
                                else if (variable == "linker_flags") {
                                    for (std::list<std::string>::const_iterator
                                            it = thekogans_make.linker_flags.begin (),
                                            end = thekogans_make.linker_flags.end (); it != end; ++it) {
                                        makefileFile << "\\\n  " << *it;
                                    }
                                    // ld needs to be able to locate the dependency libraries.
                                    if (thekogans_make.project_type == PROJECT_TYPE_PROGRAM && core::_TOOLCHAIN_OS == "Linux" &&
                                            thekogans_make.type == TYPE_SHARED) {
                                        std::set<std::string> sharedLibraries;
                                        thekogans_make.GetSharedLibraries (sharedLibraries);
                                        for (std::set<std::string>::const_iterator
                                                it = sharedLibraries.begin (),
                                                end = sharedLibraries.end (); it != end; ++it) {
                                            makefileFile << "\\\n  " << "-Wl,-rpath-link," << util::Path (*it).GetDirectory ();
                                        }
                                    }
                                }
                                else if (variable == "librarian_flags") {
                                    for (std::list<std::string>::const_iterator
                                            it = thekogans_make.librarian_flags.begin (),
                                            end = thekogans_make.librarian_flags.end (); it != end; ++it) {
                                        makefileFile << "\\\n  " << *it;
                                    }
                                }
                                else if (variable == "link_libraries") {
                                    std::list<std::string> link_libraries;
                                    thekogans_make.GetLinkLibraries (link_libraries);
                                    for (std::list<std::string>::const_iterator
                                            it = link_libraries.begin (),
                                            end = link_libraries.end (); it != end; ++it) {
                                        makefileFile << "\\\n  " << *it;
                                    }
                                }
                                else if (variable == "common_preprocessor_definitions") {
                                    std::list<std::string> preprocessor_definitions;
                                    thekogans_make.GetCommonPreprocessorDefinitions (preprocessor_definitions);
                                    for (std::list<std::string>::const_iterator
                                            it = preprocessor_definitions.begin (),
                                            end = preprocessor_definitions.end (); it != end; ++it) {
                                        makefileFile << "\\\n  " << *it;
                                    }
                                    for (std::list<std::string>::const_iterator
                                            it = thekogans_make.preprocessor_definitions.begin (),
                                            end = thekogans_make.preprocessor_definitions.end (); it != end; ++it) {
                                        makefileFile << "\\\n  " << *it;
                                    }
                                }
                                else if (variable == "features") {
                                    std::set<std::string> features;
                                    thekogans_make.GetFeatures (features);
                                    for (std::set<std::string>::const_iterator
                                            it = features.begin (),
                                            end = features.end (); it != end; ++it) {
                                        makefileFile << "\\\n  " << *it;
                                    }
                                }
                                else if (variable == "masm_flags") {
                                    for (std::list<std::string>::const_iterator
                                            it = thekogans_make.masm_flags.begin (),
                                            end = thekogans_make.masm_flags.end (); it != end; ++it) {
                                        makefileFile << "\\\n  " << *it;
                                    }
                                }
                                else if (variable == "masm_preprocessor_definitions") {
                                    for (std::list<std::string>::const_iterator
                                            it = thekogans_make.masm_preprocessor_definitions.begin (),
                                            end = thekogans_make.masm_preprocessor_definitions.end ();
                                            it != end; ++it) {
                                        makefileFile << "\\\n  " << *it;
                                    }
                                }
                                else if (variable == "masm_headers") {
                                    DumpFileLists (
                                        makefileFile,
                                        thekogans_make.masm_headers,
                                        thekogans_make,
                                        customBuildList);
                                }
                                else if (variable == "masm_sources") {
                                    DumpFileLists (
                                        makefileFile,
                                        thekogans_make.masm_sources,
                                        thekogans_make,
                                        customBuildList);
                                }
                                else if (variable == "masm_tests") {
                                    DumpFileLists (
                                        makefileFile,
                                        thekogans_make.masm_tests,
                                        thekogans_make,
                                        customBuildList);
                                }
                                else if (variable == "nasm_flags") {
                                    for (std::list<std::string>::const_iterator
                                            it = thekogans_make.nasm_flags.begin (),
                                            end = thekogans_make.nasm_flags.end (); it != end; ++it) {
                                        makefileFile << "\\\n  " << *it;
                                    }
                                }
                                else if (variable == "nasm_preprocessor_definitions") {
                                    for (std::list<std::string>::const_iterator
                                            it = thekogans_make.nasm_preprocessor_definitions.begin (),
                                            end = thekogans_make.nasm_preprocessor_definitions.end ();
                                            it != end; ++it) {
                                        makefileFile << "\\\n  " << *it;
                                    }
                                }
                                else if (variable == "nasm_headers") {
                                    DumpFileLists (
                                        makefileFile,
                                        thekogans_make.nasm_headers,
                                        thekogans_make,
                                        customBuildList);
                                }
                                else if (variable == "nasm_sources") {
                                    DumpFileLists (
                                        makefileFile,
                                        thekogans_make.nasm_sources,
                                        thekogans_make,
                                        customBuildList);
                                }
                                else if (variable == "nasm_tests") {
                                    DumpFileLists (
                                        makefileFile,
                                        thekogans_make.nasm_tests,
                                        thekogans_make,
                                        customBuildList);
                                }
                                else if (variable == "c_flags") {
                                    for (std::list<std::string>::const_iterator
                                            it = thekogans_make.c_flags.begin (),
                                            end = thekogans_make.c_flags.end (); it != end; ++it) {
                                        makefileFile << "\\\n  " << *it;
                                    }
                                }
                                else if (variable == "c_preprocessor_definitions") {
                                    for (std::list<std::string>::const_iterator
                                            it = thekogans_make.c_preprocessor_definitions.begin (),
                                            end = thekogans_make.c_preprocessor_definitions.end ();
                                            it != end; ++it) {
                                        makefileFile << "\\\n  " << *it;
                                    }
                                }
                                else if (variable == "c_headers") {
                                    DumpFileLists (
                                        makefileFile,
                                        thekogans_make.c_headers,
                                        thekogans_make,
                                        customBuildList);
                                }
                                else if (variable == "c_sources") {
                                    DumpFileLists (
                                        makefileFile,
                                        thekogans_make.c_sources,
                                        thekogans_make,
                                        customBuildList);
                                }
                                else if (variable == "c_tests") {
                                    DumpFileLists (
                                        makefileFile,
                                        thekogans_make.c_tests,
                                        thekogans_make,
                                        customBuildList);
                                }
                                else if (variable == "cpp_flags") {
                                    for (std::list<std::string>::const_iterator
                                            it = thekogans_make.cpp_flags.begin (),
                                            end = thekogans_make.cpp_flags.end (); it != end; ++it) {
                                        makefileFile << "\\\n  " << *it;
                                    }
                                }
                                else if (variable == "cpp_preprocessor_definitions") {
                                    for (std::list<std::string>::const_iterator
                                            it = thekogans_make.cpp_preprocessor_definitions.begin (),
                                            end = thekogans_make.cpp_preprocessor_definitions.end ();
                                            it != end; ++it) {
                                        makefileFile << "\\\n  " << *it;
                                    }
                                }
                                else if (variable == "cpp_headers") {
                                    DumpFileLists (
                                        makefileFile,
                                        thekogans_make.cpp_headers,
                                        thekogans_make,
                                        customBuildList);
                                }
                                else if (variable == "cpp_sources") {
                                    DumpFileLists (
                                        makefileFile,
                                        thekogans_make.cpp_sources,
                                        thekogans_make,
                                        customBuildList);
                                }
                                else if (variable == "cpp_tests") {
                                    DumpFileLists (
                                        makefileFile,
                                        thekogans_make.cpp_tests,
                                        thekogans_make,
                                        customBuildList);
                                }
                                else if (variable == "objective_c_flags") {
                                    for (std::list<std::string>::const_iterator
                                            it = thekogans_make.objective_c_flags.begin (),
                                            end = thekogans_make.objective_c_flags.end (); it != end; ++it) {
                                        makefileFile << "\\\n  " << *it;
                                    }
                                }
                                else if (variable == "objective_c_preprocessor_definitions") {
                                    for (std::list<std::string>::const_iterator
                                            it = thekogans_make.objective_c_preprocessor_definitions.begin (),
                                            end = thekogans_make.objective_c_preprocessor_definitions.end ();
                                            it != end; ++it) {
                                        makefileFile << "\\\n  " << *it;
                                    }
                                }
                                else if (variable == "objective_c_headers") {
                                    DumpFileLists (
                                        makefileFile,
                                        thekogans_make.objective_c_headers,
                                        thekogans_make,
                                        customBuildList);
                                }
                                else if (variable == "objective_c_sources") {
                                    DumpFileLists (
                                        makefileFile,
                                        thekogans_make.objective_c_sources,
                                        thekogans_make,
                                        customBuildList);
                                }
                                else if (variable == "objective_c_tests") {
                                    DumpFileLists (
                                        makefileFile,
                                        thekogans_make.objective_c_tests,
                                        thekogans_make,
                                        customBuildList);
                                }
                                else if (variable == "objective_cpp_flags") {
                                    for (std::list<std::string>::const_iterator
                                            it = thekogans_make.objective_cpp_flags.begin (),
                                            end = thekogans_make.objective_cpp_flags.end (); it != end; ++it) {
                                        makefileFile << "\\\n  " << *it;
                                    }
                                }
                                else if (variable == "objective_cpp_preprocessor_definitions") {
                                    for (std::list<std::string>::const_iterator
                                            it = thekogans_make.objective_cpp_preprocessor_definitions.begin (),
                                            end = thekogans_make.objective_cpp_preprocessor_definitions.end ();
                                            it != end; ++it) {
                                        makefileFile << "\\\n  " << *it;
                                    }
                                }
                                else if (variable == "objective_cpp_headers") {
                                    DumpFileLists (
                                        makefileFile,
                                        thekogans_make.objective_cpp_headers,
                                        thekogans_make,
                                        customBuildList);
                                }
                                else if (variable == "objective_cpp_sources") {
                                    DumpFileLists (
                                        makefileFile,
                                        thekogans_make.objective_cpp_sources,
                                        thekogans_make,
                                        customBuildList);
                                }
                                else if (variable == "objective_cpp_tests") {
                                    DumpFileLists (
                                        makefileFile,
                                        thekogans_make.objective_cpp_tests,
                                        thekogans_make,
                                        customBuildList);
                                }
                                else if (variable == "resources") {
                                    DumpFileLists (
                                        makefileFile,
                                        thekogans_make.resources,
                                        thekogans_make,
                                        customBuildList);
                                }
                                else if (variable == "rc_flags") {
                                    for (std::list<std::string>::const_iterator
                                            it = thekogans_make.rc_flags.begin (),
                                            end = thekogans_make.rc_flags.end (); it != end; ++it) {
                                        makefileFile << "\\\n  " << *it;
                                    }
                                }
                                else if (variable == "rc_preprocessor_definitions") {
                                    for (std::list<std::string>::const_iterator
                                            it = thekogans_make.rc_preprocessor_definitions.begin (),
                                            end = thekogans_make.rc_preprocessor_definitions.end ();
                                            it != end; ++it) {
                                        makefileFile << "\\\n  " << *it;
                                    }
                                }
                                else if (variable == "rc_sources") {
                                    DumpFileLists (
                                        makefileFile,
                                        thekogans_make.rc_sources,
                                        thekogans_make,
                                        customBuildList);
                                }
                                else if (variable == "subsystem") {
                                    makefileFile << thekogans_make.subsystem;
                                }
                                else if (variable == "def_file") {
                                    makefileFile << thekogans_make.def_file;
                                }
                                else if (variable == "custom_build_rules") {
                                    std::list<std::string> extra_clean;
                                    for (std::list<core::thekogans_make::FileList::File::CustomBuild::Ptr>::const_iterator
                                            it = customBuildList.begin (),
                                            end = customBuildList.end (); it != end; ++it) {
                                        for (std::size_t j = 0, count = (*it)->outputs.size (); j < count; ++j) {
                                            makefileFile << (*it)->outputs[j] << " ";
                                        }
                                        makefileFile << ":";
                                        for (std::size_t j = 0, count = (*it)->dependencies.size (); j < count; ++j) {
                                            makefileFile << " " << (*it)->dependencies[j];
                                        }
                                        if (!(*it)->recipe.empty ()) {
                                            makefileFile << "\n";
                                            makefileFile <<
                                                "\t$(hide)$(call maybe-mkdir,$(dir $@))\n" <<
                                                "\t$(hide)echo " << (!(*it)->message.empty () ? (*it)->message : "Generating $@") << "\n" <<
                                                "\t$(hide)" << (*it)->recipe << "\n";
                                            for (std::size_t j = 0, count = (*it)->outputs.size (); j < count; ++j) {
                                                extra_clean.push_back ((*it)->outputs[j]);
                                            }
                                        }
                                    }
                                    if (!extra_clean.empty ()) {
                                        makefileFile << "\nextra_clean := ";
                                        for (std::list<std::string>::const_iterator
                                                it = extra_clean.begin (),
                                                end = extra_clean.end (); it != end; ++it) {
                                            makefileFile << "\\\n  " << *it;
                                        }
                                    }
                                }
                                else {
                                    makefileFile << ch << '(' << variable << ')';
                                }
                                break;
                            }
                            case '\r': {
                                break;
                            }
                            default: {
                                makefileFile << ch;
                                break;
                            }
                        }
                    }
                    return true;
                }
                else {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Unable to open '%s'.", makefileFilePath.size ());
                }
            }
            return false;
        }

        void make::Delete (
                const std::string &project_root,
                const std::string &config,
                const std::string &type,
                bool deleteDependencies) {
            const core::thekogans_make &thekogans_make =
                core::thekogans_make::GetConfig (
                    project_root,
                    THEKOGANS_MAKE_XML,
                    GetName (),
                    config,
                    type);
            if (deleteDependencies) {
                for (std::list<core::thekogans_make::Dependency::Ptr>::const_iterator
                        it = thekogans_make.dependencies.begin (),
                        end = thekogans_make.dependencies.end (); it != end; ++it) {
                    if ((*it)->GetConfigFile () == THEKOGANS_MAKE_XML) {
                        make dependency (false);
                        dependency.Delete (
                            (*it)->GetProjectRoot (),
                            (*it)->GetConfig (),
                            (*it)->GetType (),
                            deleteDependencies);
                    }
                }
            }
            std::string build_root = core::GetBuildRoot (project_root, GetName (), config, type);
            util::Path (ToSystemPath (build_root)).Delete ();
            while (build_root != project_root) {
                build_root = util::Path (build_root).GetDirectory ();
                THEKOGANS_UTIL_TRY {
                    util::Path (ToSystemPath (build_root)).Delete (false);
                }
                THEKOGANS_UTIL_CATCH_ANY {
                    break;
                }
            }
        }

    } // namespace make
} // namespace thekogans
