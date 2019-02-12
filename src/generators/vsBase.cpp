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

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "thekogans/util/Types.h"
#include "thekogans/util/ByteSwap.h"
#include "thekogans/util/GUID.h"
#include "thekogans/util/Path.h"
#include "thekogans/util/File.h"
#include "thekogans/util/Directory.h"
#include "thekogans/util/LoggerMgr.h"
#include "thekogans/util/Exception.h"
#include "thekogans/util/StringUtils.h"
#include "thekogans/make/core/Function.h"
#include "thekogans/make/core/Utils.h"
#if defined (THEKOGANS_MAKE_HAVE_CURL)
    #include "thekogans/make/core/Sources.h"
#endif // defined (THEKOGANS_MAKE_HAVE_CURL)
#include "thekogans/make/generators/vsBase.h"

namespace thekogans {
    namespace make {

        namespace {
            // general
            const char * const RESOURCES_FOLDER = "resources";
            // .sln
            const char * const SLN_EXT = ".sln";
            const char * const SLN_PROJECT_TEMPLATE =
                "Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"%s\", \"%s\", \"{%s}\"\n";
            const char * const SLN_PROJECT_SECTION =
                "\tProjectSection(ProjectDependencies) = postProject\n";
            const char * const SLN_DEPENDENCY_TEMPLATE =
                "\t\t{%s} = {%s}\n";
            const char * const SLN_END_PROJECT_SECTION =
                "\tEndProjectSection\n";
            const char * const SLN_END_PROJECT =
                "EndProject\n";
            const char * const SLN_DEPENDENCY_TARGET_TEMPLATE_i386 =
                "\t\t{%s}.$(config) $(type)|Win32.ActiveCfg = $(config) $(type)|Win32\n"
                "\t\t{%s}.$(config) $(type)|Win32.Build.0 = $(config) $(type)|Win32\n";
            const char * const SLN_DEPENDENCY_TARGET_TEMPLATE_x86_64 =
                "\t\t{%s}.$(config) $(type)|x64.ActiveCfg = $(config) $(type)|x64\n"
                "\t\t{%s}.$(config) $(type)|x64.Build.0 = $(config) $(type)|x64\n";
            // .vcxproj
            const char * const VCXPROJ_EXT = ".vcxproj";
            const char * const VCXPROJ_PRE_BUILD_EVENT =
                "    <PreBuildEvent>\n"
                "      <Message>Update build system.</Message>\n"
                "      <Command>\"$(TOOLCHAIN_SHELL)\" \"$(TOOLCHAIN_ROOT)/common/bin/makebuild\" -g:$(generator) -pr:\"$(project_root)\" -c:$(config) -t:$(type)</Command>\n"
                "    </PreBuildEvent>\n";
            const char * const VCXPROJ_POST_BUILD_EVENT_PLUGIN =
                "    <PostBuildEvent>\n"
                "      <Message>Copy plugin.</Message>\n"
                "      <Command>\"$(TOOLCHAIN_SHELL)\" \"$(TOOLCHAIN_ROOT)/common/bin/copyplugin\" -pr:\"$(project_root)\" -c:$(config)</Command>\n"
                "    </PostBuildEvent>\n";
            const char * const VCXPROJ_POST_BUILD_EVENT_PROGRAM =
                "    <PostBuildEvent>\n"
                "      <Message>Copy dependencies.</Message>\n"
                "      <Command>\"$(TOOLCHAIN_SHELL)\" \"$(TOOLCHAIN_ROOT)/common/bin/copydependencies\" -pr:\"$(project_root)\" -c:$(config) -t:$(type)</Command>\n"
                "    </PostBuildEvent>\n";
            const char * const VCXPROJ_IMPORT_LIBRARY =
                "      <ImportLibrary>$(OutDir)$(TargetName).lib</ImportLibrary>\n";
            const char * const VCXPROJ_MODULE_DEFINITION_FILE =
                "      <ModuleDefinitionFile>%s</ModuleDefinitionFile>\n";
            const char * const VCXPROJ_SUB_SYSTEM =
                "      <SubSystem>%s</SubSystem>\n";
            const char * const VCXPROJ_HEADER_TEMPLATE =
                "    <ClInclude Include=\"%s\"/>\n";
            const char * const VCXPROJ_SOURCE_TEMPLATE =
                "    <ClCompile Include=\"%s\">\n"
                "      <ObjectFileName>$(IntDir)%s</ObjectFileName>\n"
                "    </ClCompile>\n";
            const char * const VCXPROJ_RC_SOURCE_TEMPLATE =
                "    <ResourceCompile Include=\"%s\">\n"
                "      <PreprocessorDefinitions>%s%%(PreprocessorDefinitions)</PreprocessorDefinitions>\n"
                "      <AdditionalIncludeDirectories>%s</AdditionalIncludeDirectories>\n"
                "    </ResourceCompile>";
            const char * const VCXPROJ_RESOURCE_TEMPLATE = "    <None Include=\"%s\"/>\n";
            const char * const VCXPROJ_CUSTOM_BUILD_TEMPLATE =
                "    <CustomBuild Include=\"%s\">\n"
                "      <FileType>Document</FileType>\n"
                "      <Command>%s</Command>\n"
                "      <Message>%s</Message>\n"
                "      <Outputs>%s</Outputs>\n"
                "      <AdditionalInputs>%s;%%(AdditionalInputs)</AdditionalInputs>\n"
                "    </CustomBuild>\n";
            // .vcxproj.filters
            const char * const VCXPROJ_FILTERS_EXT = ".vcxproj.filters";
            const char * const VCXPROJ_FILTERS_MASM_HEADERS_FILTER_PREFIX = "masm_headers";
            const char * const VCXPROJ_FILTERS_MASM_SOURCES_FILTER_PREFIX = "masm_sources";
            const char * const VCXPROJ_FILTERS_NASM_HEADERS_FILTER_PREFIX = "nasm_headers";
            const char * const VCXPROJ_FILTERS_NASM_SOURCES_FILTER_PREFIX = "nasm_sources";
            const char * const VCXPROJ_FILTERS_C_HEADERS_FILTER_PREFIX = "c_headers";
            const char * const VCXPROJ_FILTERS_C_SOURCES_FILTER_PREFIX = "c_sources";
            const char * const VCXPROJ_FILTERS_CPP_HEADERS_FILTER_PREFIX = "cpp_headers";
            const char * const VCXPROJ_FILTERS_CPP_SOURCES_FILTER_PREFIX = "cpp_sources";
            const char * const VCXPROJ_FILTERS_RC_SOURCES_FILTER_PREFIX = "rc_sources";
            const char * const VCXPROJ_FILTERS_RESOURCES_FILTER_PREFIX = "resources";
            const char * const VCXPROJ_FILTERS_CUSTOM_BUILD_SOURCES_FILTER_PREFIX = "custom_build_sources";
            const char * const VCXPROJ_FILTERS_FILTER_TEMPLATE =
                "    <Filter Include=\"%s\">\n"
                "      <UniqueIdentifier>{%s}</UniqueIdentifier>\n"
                "    </Filter>\n";
            const char * const VCXPROJ_FILTERS_HEADER_TEMPLATE =
                "    <ClInclude Include=\"%s\">\n"
                "      <Filter>%s</Filter>\n"
                "    </ClInclude>\n";
            const char * const VCXPROJ_FILTERS_SOURCE_TEMPLATE =
                "    <ClCompile Include=\"%s\">\n"
                "      <Filter>%s</Filter>\n"
                "    </ClCompile>\n";
            const char * const VCXPROJ_FILTERS_RC_SOURCE_TEMPLATE =
                "    <ResourceCompile Include=\"%s\">\n"
                "      <Filter>%s</Filter>\n"
                "    </ResourceCompile>\n";
            const char * const VCXPROJ_FILTERS_RESOURCE_TEMPLATE =
                "    <None Include=\"%s\">\n"
                "      <Filter>%s</Filter>\n"
                "    </None>\n";
            const char * const VCXPROJ_FILTERS_CUSTOM_BUILD_TEMPLATE =
                "    <CustomBuild Include=\"%s\">\n"
                "      <Filter>%s</Filter>\n"
                "    </CustomBuild>\n";

            inline std::string GetPlatform () {
                return
                    core::_TOOLCHAIN_ARCH == ARCH_i386 ? "Win32" :
                    core::_TOOLCHAIN_ARCH == ARCH_x86_64 ? "x64" : "";
            }

        #if defined (THEKOGANS_MAKE_USE_SHARED_RUNTIME)
            inline std::string GetRuntimeLibrary (
                    const std::string &config,
                    const std::string & /*type*/) {
                return
                    config == CONFIG_DEBUG ? "MultiThreadedDebugDLL" :
                    config == CONFIG_RELEASE ? "MultiThreadedDLL" : std::string ();
            }
        #else // defined (THEKOGANS_MAKE_USE_SHARED_RUNTIME)
            inline std::string GetRuntimeLibrary (
                    const std::string &config,
                    const std::string &type) {
                return
                    config == CONFIG_DEBUG && type == TYPE_SHARED ? "MultiThreadedDebugDLL" :
                    config == CONFIG_DEBUG && type == TYPE_STATIC ? "MultiThreadedDebug" :
                    config == CONFIG_RELEASE && type == TYPE_SHARED ? "MultiThreadedDLL" :
                    config == CONFIG_RELEASE && type == TYPE_STATIC ? "MultiThreaded" : std::string ();
            }
        #endif // defined (THEKOGANS_MAKE_USE_SHARED_RUNTIME)

            inline std::string GetTargetMachine () {
                return
                    core::_TOOLCHAIN_ARCH == ARCH_i386 ? "MachineX86" :
                    core::_TOOLCHAIN_ARCH == ARCH_x86_64 ? "MachineX64" : "";
            }

            inline const char *GetSLN_DEPENDENCY_TARGET_TEMPLATE () {
                return
                    core::_TOOLCHAIN_ARCH == ARCH_i386 ? SLN_DEPENDENCY_TARGET_TEMPLATE_i386 :
                    core::_TOOLCHAIN_ARCH == ARCH_x86_64 ? SLN_DEPENDENCY_TARGET_TEMPLATE_x86_64 : "";
            }

            inline std::string CreateRelativePath (const std::string &path) {
                // NOTE: Based on the design of the build folder:
                // $(project_root)\build\$(TOOLCHAIN_BRANCH)\generator\$(config)\$(type),
                // ..\..\..\..\..\..\.. will get us back to $(project_root).
                return "..\\..\\..\\..\\..\\..\\..\\" + path;
            }

            struct ProjectRootAndGUID {
                std::string project_root;
                std::string config;
                std::string type;
                util::GUID guid;

                ProjectRootAndGUID (
                    const std::string &project_root_,
                    const std::string &config_,
                    const std::string &type_,
                    const util::GUID &guid_) :
                    project_root (project_root_),
                    config (config_),
                    type (type_),
                    guid (guid_) {}
            };

            bool FindProjectRoot (
                    const std::list<ProjectRootAndGUID> &projectdependencies,
                    const std::string &project_root) {
                for (std::list<ProjectRootAndGUID>::const_iterator
                        jt = projectdependencies.begin (),
                        end = projectdependencies.end (); jt != end; ++jt) {
                    if ((*jt).project_root == project_root) {
                        return true;
                    }
                }
                return false;
            }

            void GetProjectDependencies (
                    const core::thekogans_make &thekogans_make,
                    std::list<ProjectRootAndGUID> &projectDependencies,
                    bool recursive = true) {
                for (std::list<core::thekogans_make::Dependency::Ptr>::const_iterator
                        it = thekogans_make.dependencies.begin (),
                        end = thekogans_make.dependencies.end (); it != end; ++it) {
                    if ((*it)->GetConfigFile () == THEKOGANS_MAKE_XML &&
                            !FindProjectRoot (projectDependencies, (*it)->GetProjectRoot ())) {
                        const core::thekogans_make &dependency = core::thekogans_make::GetConfig (
                            (*it)->GetProjectRoot (),
                            (*it)->GetConfigFile (),
                            (*it)->GetGenerator (),
                            (*it)->GetConfig (),
                            (*it)->GetType ());
                        projectDependencies.push_back (
                            ProjectRootAndGUID (
                                dependency.project_root,
                                dependency.config,
                                dependency.type,
                                dependency.guid));
                        if (recursive) {
                            GetProjectDependencies (dependency, projectDependencies, recursive);
                        }
                    }
                }
            }

            std::string GetVariable (
                    const char **buffer,
                    const char delimiters[] = "()") {
                const char *ptr = *buffer;
                if (*ptr != delimiters[0]) {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Invalid variable declaration in: %s", ptr);
                }
                ++ptr;
                std::string variable;
                while (*ptr != '\0' && *ptr != delimiters[1]) {
                    variable += *ptr++;
                }
                if (*ptr != delimiters[1]) {
                    THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                        "Invalid variable declaration in: %s", ptr);
                }
                *buffer = ++ptr;
                return variable;
            }

            inline std::string GetMasmPath () {
                static const std::string _TOOLCHAIN_VISUAL_STUDIO_DIR =
                    util::GetEnvironmentVariable ("TOOLCHAIN_VISUAL_STUDIO_DIR");
                static const std::string _TOOLCHAIN_MASM =
                    util::GetEnvironmentVariable ("TOOLCHAIN_MASM");
                return ToSystemPath (core::MakePath (_TOOLCHAIN_VISUAL_STUDIO_DIR, _TOOLCHAIN_MASM));
            }

            inline std::string GetNasmPath () {
                return core::Toolchain::GetProgram (
                    "thekogans",
                    "nasm",
                    core::Toolchain::GetLatestVersion (
                        "thekogans",
                        "nasm"));
            }
        }

        bool vsBase::Generate (
                const std::string &project_root,
                const std::string &config,
                const std::string &type,
                bool generateDependencies,
                bool force) {
            struct to_build_system_path : public core::Function {
                static core::Function::UniquePtr Create () {
                    return core::Function::UniquePtr (new to_build_system_path);
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
                    return core::Value (ToSystemPath (path));
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
            std::string vcxprojFilePath =
                ToSystemPath (
                    core::MakePath (
                        core::GetBuildRoot (project_root, GetName (), config, type),
                        GetQualifiedName (thekogans_make.organization, thekogans_make.project) + VCXPROJ_EXT));
            bool vcxprojFilePathExists = util::Path (vcxprojFilePath).Exists ();
            time_t vcxprojFilePathLastModifiedDate = 0;
            if (vcxprojFilePathExists) {
                vcxprojFilePathLastModifiedDate =
                    util::Directory::Entry (vcxprojFilePath).lastModifiedDate;
            }
            bool updatedDependency = false;
            if (generateDependencies) {
                if (thekogans_make.project_type == PROJECT_TYPE_PLUGIN) {
                    for (std::list<core::thekogans_make::Dependency::Ptr>::const_iterator
                            it = thekogans_make.plugin_hosts.begin (),
                            end = thekogans_make.plugin_hosts.end (); it != end; ++it) {
                        if ((*it)->GetConfigFile () == THEKOGANS_MAKE_XML) {
                            const core::thekogans_make &dependencyConfig = core::thekogans_make::GetConfig (
                                (*it)->GetProjectRoot (),
                                (*it)->GetConfigFile (),
                                (*it)->GetGenerator (),
                                (*it)->GetConfig (),
                                (*it)->GetType ());
                            vsBase::Ptr dependency = core::Generator::Get (GetName (), false);
                            updatedDependency =
                                dependency->Generate (
                                    (*it)->GetProjectRoot (),
                                    (*it)->GetConfig (),
                                    (*it)->GetType (),
                                    generateDependencies,
                                    force) ||
                                updatedDependency ||
                                !vcxprojFilePathExists ||
                                vcxprojFilePathLastModifiedDate <
                                util::Directory::Entry (
                                    ToSystemPath (
                                        core::MakePath (
                                            core::GetBuildRoot (
                                                (*it)->GetProjectRoot (),
                                                GetName (),
                                                (*it)->GetConfig (),
                                                (*it)->GetType ()),
                                            GetQualifiedName (
                                                dependencyConfig.organization,
                                                dependencyConfig.project) + VCXPROJ_EXT))).lastModifiedDate;
                        }
                    }
                }
                for (std::list<core::thekogans_make::Dependency::Ptr>::const_iterator
                        it = thekogans_make.dependencies.begin (),
                        end = thekogans_make.dependencies.end (); it != end; ++it) {
                    if ((*it)->GetConfigFile () == THEKOGANS_MAKE_XML) {
                        const core::thekogans_make &dependencyConfig = core::thekogans_make::GetConfig (
                            (*it)->GetProjectRoot (),
                            (*it)->GetConfigFile (),
                            (*it)->GetGenerator (),
                            (*it)->GetConfig (),
                            (*it)->GetType ());
                        vsBase::Ptr dependency = core::Generator::Get (GetName (), false);
                        updatedDependency =
                            dependency->Generate (
                                (*it)->GetProjectRoot (),
                                (*it)->GetConfig (),
                                (*it)->GetType (),
                                generateDependencies,
                                force) ||
                            updatedDependency ||
                            !vcxprojFilePathExists ||
                            vcxprojFilePathLastModifiedDate <
                                util::Directory::Entry (
                                    ToSystemPath (
                                        core::MakePath (
                                            core::GetBuildRoot (
                                                (*it)->GetProjectRoot (),
                                                GetName (),
                                                (*it)->GetConfig (),
                                                (*it)->GetType ()),
                                            GetQualifiedName (
                                                dependencyConfig.organization,
                                                dependencyConfig.project) + VCXPROJ_EXT))).lastModifiedDate;
                    }
                }
            }
            std::string thekogans_makeFilePath =
                ToSystemPath (core::MakePath (project_root, THEKOGANS_MAKE_XML));
            if (force ||
                    updatedDependency ||
                    !vcxprojFilePathExists ||
                #if defined (THEKOGANS_MAKE_HAVE_CURL)
                    vcxprojFilePathLastModifiedDate <
                        util::Directory::Entry (
                            ToSystemPath (
                                core::MakePath (
                                    core::_TOOLCHAIN_ROOT, SOURCES_XML))).lastModifiedDate ||
                #endif // defined (THEKOGANS_MAKE_HAVE_CURL)
                    vcxprojFilePathLastModifiedDate <
                        util::Directory::Entry (thekogans_makeFilePath).lastModifiedDate) {
                for (std::list<core::thekogans_make::FileList::Ptr>::const_iterator
                        it = thekogans_make.masm_headers.begin (),
                        end = thekogans_make.masm_headers.end (); it != end; ++it) {
                    for (std::list<core::thekogans_make::FileList::File::Ptr>::const_iterator
                            jt = (*it)->files.begin (),
                            end = (*it)->files.end (); jt != end; ++jt) {
                        if ((*jt)->customBuild.get () != 0) {
                            AddCustomBuildRule (thekogans_make, **jt, (*it)->prefix);
                        }
                        else {
                            masm_headers.push_back (
                                FileAndFilter (
                                    ToSystemPath (core::MakePath ((*it)->prefix, (*jt)->name)),
                                    GetFilter (VCXPROJ_FILTERS_MASM_HEADERS_FILTER_PREFIX,
                                        (*jt)->name, header_filters)));
                        }
                    }
                }
                for (std::list<core::thekogans_make::FileList::Ptr>::const_iterator
                        it = thekogans_make.masm_sources.begin (),
                        end = thekogans_make.masm_sources.end (); it != end; ++it) {
                    for (std::list<core::thekogans_make::FileList::File::Ptr>::const_iterator
                            jt = (*it)->files.begin (),
                            end = (*it)->files.end (); jt != end; ++jt) {
                        if ((*jt)->customBuild.get () != 0) {
                            AddCustomBuildRule (thekogans_make, **jt, (*it)->prefix);
                        }
                        else {
                            masm_sources.push_back (
                                FileAndFilter (
                                    ToSystemPath (core::MakePath ((*it)->prefix, (*jt)->name)),
                                    GetFilter (VCXPROJ_FILTERS_MASM_SOURCES_FILTER_PREFIX,
                                        (*jt)->name, source_filters)));
                        }
                    }
                }
                for (std::list<core::thekogans_make::FileList::Ptr>::const_iterator
                        it = thekogans_make.nasm_headers.begin (),
                        end = thekogans_make.nasm_headers.end (); it != end; ++it) {
                    for (std::list<core::thekogans_make::FileList::File::Ptr>::const_iterator
                            jt = (*it)->files.begin (),
                            end = (*it)->files.end (); jt != end; ++jt) {
                        if ((*jt)->customBuild.get () != 0) {
                            AddCustomBuildRule (thekogans_make, **jt, (*it)->prefix);
                        }
                        else {
                            nasm_headers.push_back (
                                FileAndFilter (
                                    ToSystemPath (core::MakePath ((*it)->prefix, (*jt)->name)),
                                    GetFilter (VCXPROJ_FILTERS_NASM_HEADERS_FILTER_PREFIX,
                                        (*jt)->name, header_filters)));
                        }
                    }
                }
                for (std::list<core::thekogans_make::FileList::Ptr>::const_iterator
                        it = thekogans_make.nasm_sources.begin (),
                        end = thekogans_make.nasm_sources.end (); it != end; ++it) {
                    for (std::list<core::thekogans_make::FileList::File::Ptr>::const_iterator
                            jt = (*it)->files.begin (),
                            end = (*it)->files.end (); jt != end; ++jt) {
                        if ((*jt)->customBuild.get () != 0) {
                            AddCustomBuildRule (thekogans_make, **jt, (*it)->prefix);
                        }
                        else {
                            nasm_sources.push_back (
                                FileAndFilter (
                                    ToSystemPath (core::MakePath ((*it)->prefix, (*jt)->name)),
                                    GetFilter (VCXPROJ_FILTERS_NASM_SOURCES_FILTER_PREFIX,
                                        (*jt)->name, source_filters)));
                        }
                    }
                }
                for (std::list<core::thekogans_make::FileList::Ptr>::const_iterator
                        it = thekogans_make.c_headers.begin (),
                        end = thekogans_make.c_headers.end (); it != end; ++it) {
                    for (std::list<core::thekogans_make::FileList::File::Ptr>::const_iterator
                            jt = (*it)->files.begin (),
                            end = (*it)->files.end (); jt != end; ++jt) {
                        if ((*jt)->customBuild.get () != 0) {
                            AddCustomBuildRule (thekogans_make, **jt, (*it)->prefix);
                        }
                        else {
                            c_headers.push_back (
                                FileAndFilter (
                                    ToSystemPath (core::MakePath ((*it)->prefix, (*jt)->name)),
                                    GetFilter (VCXPROJ_FILTERS_C_HEADERS_FILTER_PREFIX,
                                        (*jt)->name, header_filters)));
                        }
                    }
                }
                for (std::list<core::thekogans_make::FileList::Ptr>::const_iterator
                        it = thekogans_make.c_sources.begin (),
                        end = thekogans_make.c_sources.end (); it != end; ++it) {
                    for (std::list<core::thekogans_make::FileList::File::Ptr>::const_iterator
                            jt = (*it)->files.begin (),
                            end = (*it)->files.end (); jt != end; ++jt) {
                        if ((*jt)->customBuild.get () != 0) {
                            AddCustomBuildRule (thekogans_make, **jt, (*it)->prefix);
                        }
                        else {
                            c_sources.push_back (
                                FileAndFilter (
                                    ToSystemPath (core::MakePath ((*it)->prefix, (*jt)->name)),
                                    GetFilter (VCXPROJ_FILTERS_C_SOURCES_FILTER_PREFIX,
                                        (*jt)->name, source_filters)));
                        }
                    }
                }
                for (std::list<core::thekogans_make::FileList::Ptr>::const_iterator
                        it = thekogans_make.cpp_headers.begin (),
                        end = thekogans_make.cpp_headers.end (); it != end; ++it) {
                    for (std::list<core::thekogans_make::FileList::File::Ptr>::const_iterator
                            jt = (*it)->files.begin (),
                            end = (*it)->files.end (); jt != end; ++jt) {
                        if ((*jt)->customBuild.get () != 0) {
                            AddCustomBuildRule (thekogans_make, **jt, (*it)->prefix);
                        }
                        else {
                            cpp_headers.push_back (
                                FileAndFilter (
                                    ToSystemPath (core::MakePath ((*it)->prefix, (*jt)->name)),
                                    GetFilter (VCXPROJ_FILTERS_CPP_HEADERS_FILTER_PREFIX,
                                        (*jt)->name, header_filters)));
                        }
                    }
                }
                for (std::list<core::thekogans_make::FileList::Ptr>::const_iterator
                        it = thekogans_make.cpp_sources.begin (),
                        end = thekogans_make.cpp_sources.end (); it != end; ++it) {
                    for (std::list<core::thekogans_make::FileList::File::Ptr>::const_iterator
                            jt = (*it)->files.begin (),
                            end = (*it)->files.end (); jt != end; ++jt) {
                        if ((*jt)->customBuild.get () != 0) {
                            AddCustomBuildRule (thekogans_make, **jt, (*it)->prefix);
                        }
                        else {
                            cpp_sources.push_back (
                                FileAndFilter (
                                    ToSystemPath (core::MakePath ((*it)->prefix, (*jt)->name)),
                                    GetFilter (VCXPROJ_FILTERS_CPP_SOURCES_FILTER_PREFIX,
                                        (*jt)->name, source_filters)));
                        }
                    }
                }
                for (std::list<core::thekogans_make::FileList::Ptr>::const_iterator
                        it = thekogans_make.rc_sources.begin (),
                        end = thekogans_make.rc_sources.end (); it != end; ++it) {
                    for (std::list<core::thekogans_make::FileList::File::Ptr>::const_iterator
                            jt = (*it)->files.begin (),
                            end = (*it)->files.end (); jt != end; ++jt) {
                        if ((*jt)->customBuild.get () != 0) {
                            AddCustomBuildRule (thekogans_make, **jt, (*it)->prefix);
                        }
                        else {
                            rc_sources.push_back (
                                FileAndFilter (
                                    ToSystemPath (core::MakePath ((*it)->prefix, (*jt)->name)),
                                    GetFilter (VCXPROJ_FILTERS_RC_SOURCES_FILTER_PREFIX,
                                        (*jt)->name, rc_source_filters)));
                        }
                    }
                }
                for (std::list<core::thekogans_make::FileList::Ptr>::const_iterator
                        it = thekogans_make.resources.begin (),
                        end = thekogans_make.resources.end (); it != end; ++it) {
                    for (std::list<core::thekogans_make::FileList::File::Ptr>::const_iterator
                            jt = (*it)->files.begin (),
                            end = (*it)->files.end (); jt != end; ++jt) {
                        if ((*jt)->customBuild.get () != 0) {
                            AddCustomBuildRule (thekogans_make, **jt, (*it)->prefix);
                        }
                        else {
                            resources.push_back (
                                FileAndFilter (
                                    ToSystemPath (core::MakePath ((*it)->prefix, (*jt)->name)),
                                    GetFilter (VCXPROJ_FILTERS_RESOURCES_FILTER_PREFIX,
                                        (*jt)->name, resource_filters)));
                        }
                    }
                }
                core::CreateBuildRoot (
                    thekogans_make.project_root,
                    GetName (),
                    thekogans_make.config,
                    thekogans_make.type);
                if (rootProject) {
                    sln (thekogans_make);
                }
                vcxproj (thekogans_make);
                vcxprojfilters (thekogans_make);
                return true;
            }
            return false;
        }

        void vsBase::Delete (
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
                        vsBase::Ptr dependency = core::Generator::Get (GetName (), false);
                        dependency->Delete (
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

        namespace {
            const char *slnTemplate =
                "﻿\n"
                "Microsoft Visual Studio Solution File, Format Version $(format_version)\n"
                "# Visual Studio $(visual_studio)\n"
                "$(dependency_dependencies)\n"
                "$(project_dependencies)\n"
                "Global\n"
                "	GlobalSection(SolutionConfigurationPlatforms) = preSolution\n"
                "		$(config) $(type)|$(platform) = $(config) $(type)|$(platform)\n"
                "	EndGlobalSection\n"
                "	GlobalSection(ProjectConfigurationPlatforms) = postSolution\n"
                "$(dependency_targets)\n"
                "$(project_targets)\n"
                "	EndGlobalSection\n"
                "	GlobalSection(SolutionProperties) = preSolution\n"
                "		HideSolutionNode = FALSE\n"
                "	EndGlobalSection\n"
                "EndGlobal\n";
        }

        void vsBase::sln (const core::thekogans_make &thekogans_make) {
            std::cout << "Generating " <<
                core::MakePath (
                    core::GetBuildRoot (
                        thekogans_make.project_root,
                        GetName (),
                        thekogans_make.config,
                        thekogans_make.type),
                    GetQualifiedName (
                        thekogans_make.organization,
                        thekogans_make.project)) <<
                SLN_EXT << std::endl;
            std::cout.flush ();
            std::string slnFilePath =
                core::MakePath (
                    core::GetBuildRoot (
                        thekogans_make.project_root,
                        GetName (),
                        thekogans_make.config,
                        thekogans_make.type),
                    GetQualifiedName (
                        thekogans_make.organization,
                        thekogans_make.project) + SLN_EXT);
            std::fstream slnFile (
                ToSystemPath (slnFilePath).c_str (),
                std::fstream::out | std::fstream::binary | std::fstream::trunc);
            if (slnFile.is_open ()) {
                std::list<ProjectRootAndGUID> projectDependencies;
                GetProjectDependencies (thekogans_make, projectDependencies);
                const char *fileTemplate = slnTemplate;
                while (*fileTemplate != '\0') {
                    util::i8 ch = *fileTemplate++;
                    if (ch == '$') {
                        std::string variable = GetVariable (&fileTemplate);
                        if (variable == "format_version") {
                            slnFile << slnGetFormatVersion ();
                        }
                        else if (variable == "visual_studio") {
                            slnFile << slnGetVisualStudio ();
                        }
                        else if (variable == "config") {
                            slnFile << thekogans_make.config;
                        }
                        else if (variable == "type") {
                            slnFile << thekogans_make.type;
                        }
                        else if (variable == "platform") {
                            slnFile << GetPlatform ();
                        }
                        else if (variable == "dependency_dependencies") {
                            for (std::list<ProjectRootAndGUID>::const_iterator
                                    it = projectDependencies.begin (),
                                    end = projectDependencies.end (); it != end; ++it) {
                                const core::thekogans_make &dependency = core::thekogans_make::GetConfig (
                                    (*it).project_root,
                                    THEKOGANS_MAKE_XML,
                                    GetName (),
                                    (*it).config,
                                    (*it).type);
                                slnFile << util::FormatString (
                                    SLN_PROJECT_TEMPLATE,
                                    GetQualifiedName (dependency.organization, dependency.project).c_str (),
                                    ToSystemPath (
                                        core::MakePath (
                                            core::GetBuildRoot (
                                                dependency.project_root,
                                                GetName (),
                                                dependency.config,
                                                dependency.type),
                                            GetQualifiedName (
                                                dependency.organization,
                                                dependency.project) + VCXPROJ_EXT)).c_str (),
                                    dependency.guid.ToWindowsGUIDString (true).c_str ());
                                std::list<ProjectRootAndGUID> dependencyDependencies;
                                GetProjectDependencies (dependency, dependencyDependencies, false);
                                if (!dependencyDependencies.empty ()) {
                                    slnFile << SLN_PROJECT_SECTION;
                                    for (std::list<ProjectRootAndGUID>::const_iterator
                                            jt = dependencyDependencies.begin (),
                                            end = dependencyDependencies.end (); jt != end; ++jt) {
                                        std::string guidString = (*jt).guid.ToWindowsGUIDString (true);
                                        slnFile << util::FormatString (
                                            SLN_DEPENDENCY_TEMPLATE,
                                            guidString.c_str (),
                                            guidString.c_str ());
                                    }
                                    slnFile << SLN_END_PROJECT_SECTION;
                                }
                                slnFile << SLN_END_PROJECT;
                            }
                        }
                        else if (variable == "project_dependencies") {
                            slnFile << util::FormatString (
                                SLN_PROJECT_TEMPLATE,
                                GetQualifiedName (thekogans_make.organization, thekogans_make.project).c_str (),
                                ToSystemPath (
                                    core::MakePath (
                                        core::GetBuildRoot (
                                            thekogans_make.project_root,
                                            GetName (),
                                            thekogans_make.config,
                                            thekogans_make.type),
                                        GetQualifiedName (
                                            thekogans_make.organization,
                                            thekogans_make.project) + VCXPROJ_EXT)).c_str (),
                                thekogans_make.guid.ToWindowsGUIDString (true).c_str ());
                            std::list<ProjectRootAndGUID> projectDependencies;
                            GetProjectDependencies (thekogans_make, projectDependencies, false);
                            if (!projectDependencies.empty ()) {
                                slnFile << SLN_PROJECT_SECTION;
                                for (std::list<ProjectRootAndGUID>::const_iterator
                                        it = projectDependencies.begin (),
                                        end = projectDependencies.end (); it != end; ++it) {
                                    std::string guidString = (*it).guid.ToWindowsGUIDString (true);
                                    slnFile << util::FormatString (
                                        SLN_DEPENDENCY_TEMPLATE,
                                        guidString.c_str (),
                                        guidString.c_str ());
                                }
                                slnFile << SLN_END_PROJECT_SECTION;
                            }
                            slnFile << SLN_END_PROJECT;
                        }
                        else if (variable == "dependency_targets") {
                            for (std::list<ProjectRootAndGUID>::const_iterator
                                    it = projectDependencies.begin (),
                                    end = projectDependencies.end (); it != end; ++it) {
                                std::string guidString = (*it).guid.ToWindowsGUIDString (true);
                                slnFile << thekogans_make.Expand (
                                    util::FormatString (
                                        GetSLN_DEPENDENCY_TARGET_TEMPLATE (),
                                        guidString.c_str (),
                                        guidString.c_str ()).c_str ());
                            }
                        }
                        else if (variable == "project_targets") {
                            std::string guidString = thekogans_make.guid.ToWindowsGUIDString (true);
                            slnFile << thekogans_make.Expand (
                                util::FormatString (
                                    GetSLN_DEPENDENCY_TARGET_TEMPLATE (),
                                    guidString.c_str (),
                                    guidString.c_str ()).c_str ());
                        }
                        else {
                            slnFile << ch << '(' << variable << ')';
                        }
                    }
                    else {
                        slnFile << ch;
                    }
                }
            }
            else {
                THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                    "Unable to open '%s'.", slnFilePath.size ());
            }
        }

        namespace {
            const char *vcxprojTemplate =
                "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                "<Project DefaultTargets=\"Build\" ToolsVersion=\"$(tools_version)\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n"
                "  <ItemGroup Label=\"ProjectConfigurations\">\n"
                "    <ProjectConfiguration Include=\"$(config) $(type)|$(platform)\">\n"
                "      <Configuration>$(config) $(type)</Configuration>\n"
                "      <Platform>$(platform)</Platform>\n"
                "    </ProjectConfiguration>\n"
                "  </ItemGroup>\n"
                "  <PropertyGroup Label=\"Globals\">\n"
                "    <ProjectGuid>{$(project_guid)}</ProjectGuid>\n"
                "    <RootNamespace>$(project)</RootNamespace>\n"
                "  </PropertyGroup>\n"
                "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.Default.props\"/>\n"
                "  <PropertyGroup Label=\"Configuration\">\n"
                "    <ConfigurationType>$(configuration_type)</ConfigurationType>\n"
                "    <UseDebugLibraries>$(use_debug_libraries)</UseDebugLibraries>\n"
                "    <WholeProgramOptimization>$(whole_program_optimization)</WholeProgramOptimization>\n"
                "    <CharacterSet>MultiByte</CharacterSet>\n"
                "    <PlatformToolset>$(platform_toolset)</PlatformToolset>\n"
                "  </PropertyGroup>\n"
                "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\"/>\n"
                "  <ImportGroup Label=\"ExtensionSettings\">\n"
                "  </ImportGroup>\n"
                "  <ImportGroup Label=\"PropertySheets\">\n"
                "    <Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\"/>\n"
                "  </ImportGroup>\n"
                "  <PropertyGroup Label=\"UserMacros\"/>\n"
                "  <PropertyGroup>\n"
                "    <TargetName>$(target_prefix)$(target_name)</TargetName>\n"
                "    <TargetExt>$(target_ext)</TargetExt>\n"
                "    <OutDir>$(project_root)\\$(out_dir)\\$(naming_convention_prefix)</OutDir>\n"
                "    <IntDir>obj\\</IntDir>\n"
                "    <IgnoreImportLibrary>true</IgnoreImportLibrary>\n"
                "    <LinkIncremental>false</LinkIncremental>\n"
                "  </PropertyGroup>\n"
                "  <ItemDefinitionGroup>\n"
                "    <ClCompile>\n"
                "      <WarningLevel>Level3</WarningLevel>\n"
                "      <Optimization>$(optimization)</Optimization>\n"
                "      <FunctionLevelLinking>$(function_level_linking)</FunctionLevelLinking>\n"
                "      <IntrinsicFunctions>$(intrinsic_functions)</IntrinsicFunctions>\n"
                "      <AdditionalIncludeDirectories>$(include_directories)</AdditionalIncludeDirectories>\n"
                "      <PreprocessorDefinitions>$(preprocessor_definitions);$(features);%(PreprocessorDefinitions)</PreprocessorDefinitions>\n"
                "      <RuntimeLibrary>$(runtime_library)</RuntimeLibrary>\n"
                "      <DebugInformationFormat>$(debug_information_format)</DebugInformationFormat>\n"
                "      <MultiProcessorCompilation>true</MultiProcessorCompilation>\n"
                "    </ClCompile>\n"
                "    <Link>\n"
                "      <TargetMachine>$(target_machine)</TargetMachine>\n"
                "      <GenerateDebugInformation>$(generate_debug_information)</GenerateDebugInformation>\n"
                "      <AdditionalDependencies>$(link_libraries)%(AdditionalDependencies)</AdditionalDependencies>\n"
                "$(import_library)\n"
                "$(module_definition_file)\n"
                "$(sub_system)\n"
                "    </Link>\n"
                "$(pre_build_event)\n"
                "$(post_build_event)\n"
                "  </ItemDefinitionGroup>\n"
                "  <ItemGroup>\n"
                "$(masm_headers)\n"
                "  </ItemGroup>\n"
                "  <ItemGroup>\n"
                "$(masm_sources)\n"
                "  </ItemGroup>\n"
                "  <ItemGroup>\n"
                "$(nasm_headers)\n"
                "  </ItemGroup>\n"
                "  <ItemGroup>\n"
                "$(nasm_sources)\n"
                "  </ItemGroup>\n"
                "  <ItemGroup>\n"
                "$(c_headers)\n"
                "  </ItemGroup>\n"
                "  <ItemGroup>\n"
                "$(c_sources)\n"
                "  </ItemGroup>\n"
                "  <ItemGroup>\n"
                "$(cpp_headers)\n"
                "  </ItemGroup>\n"
                "  <ItemGroup>\n"
                "$(cpp_sources)\n"
                "  </ItemGroup>\n"
                "  <ItemGroup>\n"
                "$(rc_sources)\n"
                "  </ItemGroup>\n"
                "  <ItemGroup>\n"
                "$(resources)\n"
                "  </ItemGroup>\n"
                "  <ItemGroup>\n"
                "$(custom_build_sources)\n"
                "  </ItemGroup>\n"
                "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\"/>\n"
                "  <ImportGroup Label=\"ExtensionTargets\">\n"
                "  </ImportGroup>\n"
                "</Project>\n";
        }

        void vsBase::vcxproj (const core::thekogans_make &thekogans_make) {
            std::cout << "Generating " <<
                core::MakePath (
                    core::GetBuildRoot (
                        thekogans_make.project_root,
                        GetName (),
                        thekogans_make.config,
                        thekogans_make.type),
                    GetQualifiedName (
                        thekogans_make.organization,
                        thekogans_make.project) + VCXPROJ_EXT) << std::endl;
            std::cout.flush ();
            std::string vcxprojFilePath =
                core::MakePath (
                    core::GetBuildRoot (
                        thekogans_make.project_root,
                        GetName (),
                        thekogans_make.config,
                        thekogans_make.type),
                    GetQualifiedName (
                        thekogans_make.organization,
                        thekogans_make.project) + VCXPROJ_EXT);
            std::fstream vcxprojFile (
                ToSystemPath (vcxprojFilePath).c_str (),
                std::fstream::out | std::fstream::binary | std::fstream::trunc);
            if (vcxprojFile.is_open ()) {
                const char *fileTemplate = vcxprojTemplate;
                while (*fileTemplate != '\0') {
                    util::i8 ch = *fileTemplate++;
                    if (ch == '$') {
                        std::string variable = GetVariable (&fileTemplate);
                        if (variable == "tools_version") {
                            vcxprojFile << vcxprojGetToolsVersion ();
                        }
                        else if (variable == "platform_toolset") {
                            vcxprojFile << vcxprojGetPlatformToolset ();
                        }
                        else if (variable == "project_guid") {
                            vcxprojFile << thekogans_make.guid.ToWindowsGUIDString (true);
                        }
                        else if (variable == "project_root") {
                            vcxprojFile << ToSystemPath (thekogans_make.project_root);
                        }
                        else if (variable == "organization") {
                            vcxprojFile << thekogans_make.organization;
                        }
                        else if (variable == "project") {
                            vcxprojFile << GetQualifiedName (thekogans_make.organization, thekogans_make.project);
                        }
                        else if (variable == "target_prefix") {
                            if (thekogans_make.project_type == PROJECT_TYPE_LIBRARY) {
                                vcxprojFile << core::LIB_PREFIX;
                            }
                        }
                        else if (variable == "target_name") {
                            vcxprojFile << thekogans_make.Expand (
                                thekogans_make.naming_convention == NAMING_CONVENTION_FLAT ?
                                "$(organization)_$(project)-$(TOOLCHAIN_TRIPLET)-$(config)-$(type).$(version)" :
                                "$(organization)_$(project).$(version)");
                        }
                        else if (variable == "target_ext") {
                            if (thekogans_make.project_type == PROJECT_TYPE_LIBRARY) {
                                if (thekogans_make.type == TYPE_STATIC) {
                                    vcxprojFile << ".lib";
                                }
                                else if (thekogans_make.type == TYPE_SHARED) {
                                    vcxprojFile << ".dll";
                                }
                            }
                            else if (thekogans_make.project_type == PROJECT_TYPE_PLUGIN) {
                                vcxprojFile << ".dll";
                            }
                            else if (thekogans_make.project_type == PROJECT_TYPE_PROGRAM) {
                                vcxprojFile << ".exe";
                            }
                        }
                        else if (variable == "out_dir") {
                            if (thekogans_make.project_type == PROJECT_TYPE_LIBRARY ||
                                    thekogans_make.project_type == PROJECT_TYPE_PLUGIN) {
                                vcxprojFile << core::LIB_DIR;
                            }
                            else if (thekogans_make.project_type == PROJECT_TYPE_PROGRAM) {
                                vcxprojFile << core::BIN_DIR;
                            }
                        }
                        else if (variable == "naming_convention_prefix") {
                            if (thekogans_make.naming_convention == NAMING_CONVENTION_HIERARCHICAL) {
                                std::string naming_convention_prefix =
                                    thekogans_make.Expand ("$(TOOLCHAIN_BRANCH)/$(config)/$(type)/");
                                std::replace (
                                    naming_convention_prefix.begin (),
                                    naming_convention_prefix.end (), '/', '\\');
                                vcxprojFile << naming_convention_prefix;
                            }
                        }
                        else if (variable == "generator") {
                            vcxprojFile << GetName ();
                        }
                        else if (variable == "target_machine") {
                            vcxprojFile << GetTargetMachine ();
                        }
                        else if (variable == "TOOLCHAIN_SHELL") {
                            vcxprojFile << ToSystemPath (core::_TOOLCHAIN_SHELL);
                        }
                        else if (variable == "TOOLCHAIN_ROOT") {
                            vcxprojFile << core::_TOOLCHAIN_ROOT;
                        }
                        else if (variable == "TOOLCHAIN_NAMING_CONVENTION") {
                            vcxprojFile << core::_TOOLCHAIN_NAMING_CONVENTION;
                        }
                        else if (variable == "TOOLCHAIN_TRIPLET") {
                            vcxprojFile << core::_TOOLCHAIN_TRIPLET;
                        }
                        else if (variable == "version") {
                            vcxprojFile << thekogans_make.GetVersion ();
                        }
                        else if (variable == "config") {
                            vcxprojFile << thekogans_make.config;
                        }
                        else if (variable == "type") {
                            vcxprojFile << thekogans_make.type;
                        }
                        else if (variable == "platform") {
                            vcxprojFile << GetPlatform ();
                        }
                        else if (variable == "configuration_type") {
                            if (thekogans_make.project_type == PROJECT_TYPE_LIBRARY) {
                                if (thekogans_make.type == TYPE_SHARED) {
                                    vcxprojFile << "DynamicLibrary";
                                }
                                else if (thekogans_make.type == TYPE_STATIC) {
                                    vcxprojFile << "StaticLibrary";
                                }
                            }
                            else if (thekogans_make.project_type == PROJECT_TYPE_PLUGIN) {
                                vcxprojFile << "DynamicLibrary";
                            }
                            else if (thekogans_make.project_type == PROJECT_TYPE_PROGRAM) {
                                vcxprojFile << "Application";
                            }
                        }
                        else if (variable == "use_debug_libraries") {
                            if (thekogans_make.config == CONFIG_DEBUG) {
                                vcxprojFile << util::XML_TRUE;
                            }
                            else if (thekogans_make.config == CONFIG_RELEASE) {
                                vcxprojFile << util::XML_FALSE;
                            }
                        }
                        else if (variable == "whole_program_optimization") {
                            if (thekogans_make.config == CONFIG_DEBUG) {
                                vcxprojFile << util::XML_FALSE;
                            }
                            else if (thekogans_make.config == CONFIG_RELEASE) {
                                vcxprojFile << util::XML_TRUE;
                            }
                        }
                        else if (variable == "optimization") {
                            if (thekogans_make.config == CONFIG_DEBUG) {
                                vcxprojFile << "Disabled";
                            }
                            else if (thekogans_make.config == CONFIG_RELEASE) {
                                vcxprojFile << "MaxSpeed";
                            }
                        }
                        else if (variable == "function_level_linking") {
                            if (thekogans_make.config == CONFIG_DEBUG) {
                                vcxprojFile << util::XML_FALSE;
                            }
                            else if (thekogans_make.config == CONFIG_RELEASE) {
                                vcxprojFile << util::XML_TRUE;
                            }
                        }
                        else if (variable == "intrinsic_functions") {
                            if (thekogans_make.config == CONFIG_DEBUG) {
                                vcxprojFile << util::XML_FALSE;
                            }
                            else if (thekogans_make.config == CONFIG_RELEASE) {
                                vcxprojFile << util::XML_TRUE;
                            }
                        }
                        else if (variable == "generate_debug_information") {
                            if (thekogans_make.config == CONFIG_DEBUG) {
                                vcxprojFile << util::XML_TRUE;
                            }
                            else if (thekogans_make.config == CONFIG_RELEASE) {
                                vcxprojFile << util::XML_FALSE;
                            }
                        }
                        else if (variable == "debug_information_format") {
                            if (thekogans_make.config == CONFIG_DEBUG) {
                                vcxprojFile << "OldStyle";
                            }
                            else if (thekogans_make.config == CONFIG_RELEASE) {
                                vcxprojFile << "None";
                            }
                        }
                        else if (variable == "pre_build_event") {
                            vcxprojFile << thekogans_make.Expand (VCXPROJ_PRE_BUILD_EVENT);
                        }
                        else if (variable == "post_build_event") {
                            if (thekogans_make.project_type == PROJECT_TYPE_PLUGIN) {
                                vcxprojFile << thekogans_make.Expand (VCXPROJ_POST_BUILD_EVENT_PLUGIN);
                            }
                            else if (thekogans_make.project_type == PROJECT_TYPE_PROGRAM) {
                                vcxprojFile << thekogans_make.Expand (VCXPROJ_POST_BUILD_EVENT_PROGRAM);
                            }
                        }
                        else if (variable == "include_directories") {
                            std::set<std::string> include_directories;
                            thekogans_make.GetIncludeDirectories (include_directories);
                            for (std::set<std::string>::const_iterator
                                    it = include_directories.begin (),
                                    end = include_directories.end (); it != end; ++it) {
                                vcxprojFile << ToSystemPath (*it) << ';';
                            }
                        }
                        else if (variable == "preprocessor_definitions") {
                            std::list<std::string> preprocessor_definitions;
                            if (thekogans_make.project_type == PROJECT_TYPE_PROGRAM) {
                                preprocessor_definitions.push_back (
                                    thekogans_make.subsystem == "Console" ? "_CONSOLE" : "_WINDOWS");
                            }
                            if (core::_TOOLCHAIN_ARCH == ARCH_i386) {
                                preprocessor_definitions.push_back ("WIN32");
                            }
                            else if (core::_TOOLCHAIN_ARCH == ARCH_x86_64) {
                                preprocessor_definitions.push_back ("WIN64");
                            }
                            if (thekogans_make.config == CONFIG_DEBUG) {
                                preprocessor_definitions.push_back ("_DEBUG");
                            }
                            else if (thekogans_make.config == CONFIG_RELEASE) {
                                preprocessor_definitions.push_back ("NDEBUG");
                            }
                            preprocessor_definitions.push_back ("BOOST_ALL_NO_LIB");
                            thekogans_make.GetCommonPreprocessorDefinitions (preprocessor_definitions);
                            for (std::list<std::string>::const_iterator
                                    it = preprocessor_definitions.begin (),
                                    end = preprocessor_definitions.end (); it != end; ++it) {
                                vcxprojFile << *it << ';';
                            }
                            for (std::list<std::string>::const_iterator
                                    it = thekogans_make.preprocessor_definitions.begin (),
                                    end = thekogans_make.preprocessor_definitions.end (); it != end; ++it) {
                                vcxprojFile << *it << ';';
                            }
                            for (std::list<std::string>::const_iterator
                                    it = thekogans_make.c_preprocessor_definitions.begin (),
                                    end = thekogans_make.c_preprocessor_definitions.end (); it != end; ++it) {
                                vcxprojFile << *it << ';';
                            }
                            for (std::list<std::string>::const_iterator
                                    it = thekogans_make.cpp_preprocessor_definitions.begin (),
                                    end = thekogans_make.cpp_preprocessor_definitions.end (); it != end; ++it) {
                                vcxprojFile << *it << ';';
                            }
                        }
                        else if (variable == "features") {
                            std::set<std::string> features;
                            thekogans_make.GetFeatures (features);
                            for (std::set<std::string>::const_iterator
                                    it = features.begin (),
                                    end = features.end (); it != end; ++it) {
                                vcxprojFile << *it << ';';
                            }
                        }
                        else if (variable == "runtime_library") {
                            vcxprojFile << GetRuntimeLibrary (thekogans_make.config, thekogans_make.type);
                        }
                        else if (variable == "link_libraries") {
                            std::list<std::string> link_libraries;
                            thekogans_make.GetLinkLibraries (link_libraries);
                            for (std::list<std::string>::const_iterator
                                    it = link_libraries.begin (),
                                    end = link_libraries.end (); it != end; ++it) {
                                vcxprojFile << ToSystemPath (*it) << ';';
                            }
                        }
                        else if (variable == "sub_system") {
                            if (thekogans_make.project_type == PROJECT_TYPE_PROGRAM) {
                                vcxprojFile << util::FormatString (
                                    VCXPROJ_SUB_SYSTEM,
                                    thekogans_make.subsystem == "Console" ? "Console" : "Windows");
                            }
                        }
                        else if (variable == "import_library") {
                            if (thekogans_make.project_type == PROJECT_TYPE_LIBRARY) {
                                vcxprojFile << VCXPROJ_IMPORT_LIBRARY;
                            }
                        }
                        else if (variable == "module_definition_file") {
                            if ((thekogans_make.project_type == PROJECT_TYPE_LIBRARY ||
                                    thekogans_make.project_type == PROJECT_TYPE_PLUGIN) &&
                                    !thekogans_make.def_file.empty ()) {
                                vcxprojFile << util::FormatString (
                                    VCXPROJ_MODULE_DEFINITION_FILE,
                                    ToSystemPath (thekogans_make.def_file).c_str ());
                            }
                        }
                        else if (variable == "masm_headers") {
                            for (std::list<FileAndFilter>::const_iterator
                                    it = masm_headers.begin (),
                                    end = masm_headers.end (); it != end; ++it) {
                                vcxprojFile << util::FormatString (
                                    VCXPROJ_HEADER_TEMPLATE,
                                    CreateRelativePath ((*it).first).c_str ());
                            }
                        }
                        else if (variable == "masm_sources") {
                            for (std::list<FileAndFilter>::const_iterator
                                    it = masm_sources.begin (),
                                    end = masm_sources.end (); it != end; ++it) {
                                vcxprojFile << util::FormatString (
                                    VCXPROJ_CUSTOM_BUILD_TEMPLATE,
                                    CreateRelativePath ((*it).first).c_str (),
                                    GetMasmCommandLine (thekogans_make).c_str (),
                                    "Performing Custom Build Step on \"%(Identity)\"",
                                    "$(ProjectDir)$(IntDir)%(Filename).obj", "");
                            }
                        }
                        else if (variable == "nasm_headers") {
                            for (std::list<FileAndFilter>::const_iterator
                                    it = nasm_headers.begin (),
                                    end = nasm_headers.end (); it != end; ++it) {
                                vcxprojFile << util::FormatString (
                                    VCXPROJ_HEADER_TEMPLATE,
                                    CreateRelativePath ((*it).first).c_str ());
                            }
                        }
                        else if (variable == "nasm_sources") {
                            for (std::list<FileAndFilter>::const_iterator
                                    it = nasm_sources.begin (),
                                    end = nasm_sources.end (); it != end; ++it) {
                                vcxprojFile << util::FormatString (
                                    VCXPROJ_CUSTOM_BUILD_TEMPLATE,
                                    CreateRelativePath ((*it).first).c_str (),
                                    GetNasmCommandLine (thekogans_make).c_str (),
                                    "Performing Custom Build Step on \"%(Identity)\"",
                                    "$(ProjectDir)$(IntDir)%(Filename).obj", "");
                            }
                        }
                        else if (variable == "c_headers") {
                            for (std::list<FileAndFilter>::const_iterator
                                    it = c_headers.begin (),
                                    end = c_headers.end (); it != end; ++it) {
                                vcxprojFile << util::FormatString (
                                    VCXPROJ_HEADER_TEMPLATE,
                                    CreateRelativePath ((*it).first).c_str ());
                            }
                        }
                        else if (variable == "c_sources") {
                            for (std::list<FileAndFilter>::const_iterator
                                    it = c_sources.begin (),
                                    end = c_sources.end (); it != end; ++it) {
                                vcxprojFile << util::FormatString (
                                    VCXPROJ_SOURCE_TEMPLATE,
                                    CreateRelativePath ((*it).first).c_str (),
                                    util::Path ((*it).first).GetDirectory (true).c_str ());
                            }
                        }
                        else if (variable == "cpp_headers") {
                            for (std::list<FileAndFilter>::const_iterator
                                    it = cpp_headers.begin (),
                                    end = cpp_headers.end (); it != end; ++it) {
                                vcxprojFile << util::FormatString (
                                    VCXPROJ_HEADER_TEMPLATE,
                                    CreateRelativePath ((*it).first).c_str ());
                            }
                        }
                        else if (variable == "cpp_sources") {
                            for (std::list<FileAndFilter>::const_iterator
                                    it = cpp_sources.begin (),
                                    end = cpp_sources.end (); it != end; ++it) {
                                vcxprojFile << util::FormatString (
                                    VCXPROJ_SOURCE_TEMPLATE,
                                    CreateRelativePath ((*it).first).c_str (),
                                    util::Path ((*it).first).GetDirectory (true).c_str ());
                            }
                        }
                        else if (variable == "rc_sources") {
                            for (std::list<FileAndFilter>::const_iterator
                                    it = rc_sources.begin (),
                                    end = rc_sources.end (); it != end; ++it) {
                                vcxprojFile << util::FormatString (
                                    VCXPROJ_RC_SOURCE_TEMPLATE,
                                    CreateRelativePath ((*it).first).c_str (),
                                    GetRCPreprocessorDefinitions (thekogans_make).c_str (),
                                    GetRCIncludeDirectories (thekogans_make).c_str ());
                            }
                        }
                        else if (variable == "resources") {
                            for (std::list<FileAndFilter>::const_iterator
                                    it = resources.begin (),
                                    end = resources.end (); it != end; ++it) {
                                vcxprojFile << util::FormatString (
                                    VCXPROJ_RESOURCE_TEMPLATE,
                                    CreateRelativePath ((*it).first).c_str ());
                            }
                        }
                        else if (variable == "custom_build_sources") {
                            for (std::list<FileAndFilter>::const_iterator
                                    it = custom_build_sources.begin (),
                                    end = custom_build_sources.end (); it != end; ++it) {
                                vcxprojFile << (*it).first;
                            }
                        }
                        else {
                            vcxprojFile << ch << '(' << variable << ')';
                        }
                    }
                    else {
                        vcxprojFile << ch;
                    }
                }
            }
            else {
                THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                    "Unable to open '%s'.", vcxprojFilePath.size ());
            }
        }

        namespace {
            const char *vcxprojfiltersTemplate =
                "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                "<Project ToolsVersion=\"$(tools_version)\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n"
                "  <ItemGroup>\n"
                "$(filters)\n"
                "  </ItemGroup>\n"
                "  <ItemGroup>\n"
                "$(masm_headers)\n"
                "  </ItemGroup>\n"
                "  <ItemGroup>\n"
                "$(masm_sources)\n"
                "  </ItemGroup>\n"
                "  <ItemGroup>\n"
                "$(nasm_headers)\n"
                "  </ItemGroup>\n"
                "  <ItemGroup>\n"
                "$(nasm_sources)\n"
                "  </ItemGroup>\n"
                "  <ItemGroup>\n"
                "$(c_headers)\n"
                "  </ItemGroup>\n"
                "  <ItemGroup>\n"
                "$(c_sources)\n"
                "  </ItemGroup>\n"
                "  <ItemGroup>\n"
                "$(cpp_headers)\n"
                "  </ItemGroup>\n"
                "  <ItemGroup>\n"
                "$(cpp_sources)\n"
                "  </ItemGroup>\n"
                "  <ItemGroup>\n"
                "$(rc_sources)\n"
                "  </ItemGroup>\n"
                "  <ItemGroup>\n"
                "$(resources)\n"
                "  </ItemGroup>\n"
                "  <ItemGroup>\n"
                "$(custom_build_sources)\n"
                "  </ItemGroup>\n"
                "</Project>\n";
        }

        void vsBase::vcxprojfilters (const core::thekogans_make &thekogans_make) {
            std::cout << "Generating " <<
                core::MakePath (
                    core::GetBuildRoot (
                        thekogans_make.project_root,
                        GetName (),
                        thekogans_make.config,
                        thekogans_make.type),
                    GetQualifiedName (
                        thekogans_make.organization,
                        thekogans_make.project) + VCXPROJ_FILTERS_EXT) << std::endl;
            std::cout.flush ();
            std::string vcxprojfiltersFilePath =
                core::MakePath (
                    core::GetBuildRoot (
                        thekogans_make.project_root,
                        GetName (),
                        thekogans_make.config,
                        thekogans_make.type),
                    GetQualifiedName (
                        thekogans_make.organization,
                        thekogans_make.project) + VCXPROJ_FILTERS_EXT);
            std::fstream vcxprojfiltersFile (
                ToSystemPath (vcxprojfiltersFilePath).c_str (),
                std::fstream::out | std::fstream::binary | std::fstream::trunc);
            if (vcxprojfiltersFile.is_open ()) {
                const char *fileTemplate = vcxprojfiltersTemplate;
                while (*fileTemplate != '\0') {
                    util::i8 ch = *fileTemplate++;
                    if (ch == '$') {
                        std::string variable = GetVariable (&fileTemplate);
                        if (variable == "tools_version") {
                            vcxprojfiltersFile << vcxprojfiltersGetToolsVersion ();
                        }
                        else if (variable == "filters") {
                            for (std::set<std::string>::const_iterator
                                    it = header_filters.begin (),
                                    end = header_filters.end (); it != end; ++it) {
                                vcxprojfiltersFile << util::FormatString (
                                    VCXPROJ_FILTERS_FILTER_TEMPLATE,
                                    (*it).c_str (),
                                    util::GUID::FromRandom ().ToWindowsGUIDString (true).c_str ());
                            }
                            for (std::set<std::string>::const_iterator
                                    it = source_filters.begin (),
                                    end = source_filters.end (); it != end; ++it) {
                                vcxprojfiltersFile << util::FormatString (
                                    VCXPROJ_FILTERS_FILTER_TEMPLATE,
                                    (*it).c_str (),
                                    util::GUID::FromRandom ().ToWindowsGUIDString (true).c_str ());
                            }
                            for (std::set<std::string>::const_iterator
                                    it = rc_source_filters.begin (),
                                    end = rc_source_filters.end (); it != end; ++it) {
                                vcxprojfiltersFile << util::FormatString (
                                    VCXPROJ_FILTERS_FILTER_TEMPLATE,
                                    (*it).c_str (),
                                    util::GUID::FromRandom ().ToWindowsGUIDString (true).c_str ());
                            }
                            for (std::set<std::string>::const_iterator
                                    it = resource_filters.begin (),
                                    end = resource_filters.end (); it != end; ++it) {
                                vcxprojfiltersFile << util::FormatString (
                                    VCXPROJ_FILTERS_FILTER_TEMPLATE,
                                    (*it).c_str (),
                                    util::GUID::FromRandom ().ToWindowsGUIDString (true).c_str ());
                            }
                            for (std::set<std::string>::const_iterator
                                    it = custom_build_filters.begin (),
                                    end = custom_build_filters.end (); it != end; ++it) {
                                vcxprojfiltersFile << util::FormatString (
                                    VCXPROJ_FILTERS_FILTER_TEMPLATE,
                                    (*it).c_str (),
                                    util::GUID::FromRandom ().ToWindowsGUIDString (true).c_str ());
                            }
                        }
                        else if (variable == "masm_headers") {
                            for (std::list<FileAndFilter>::const_iterator
                                    it = masm_headers.begin (),
                                    end = masm_headers.end (); it != end; ++it) {
                                vcxprojfiltersFile << util::FormatString (
                                    VCXPROJ_FILTERS_HEADER_TEMPLATE,
                                    CreateRelativePath ((*it).first).c_str (),
                                    (*it).second.c_str ());
                            }
                        }
                        else if (variable == "masm_sources") {
                            for (std::list<FileAndFilter>::const_iterator
                                    it = masm_sources.begin (),
                                    end = masm_sources.end (); it != end; ++it) {
                                vcxprojfiltersFile << util::FormatString (
                                    VCXPROJ_FILTERS_CUSTOM_BUILD_TEMPLATE,
                                    CreateRelativePath ((*it).first).c_str (),
                                    (*it).second.c_str ());
                            }
                        }
                        else if (variable == "nasm_headers") {
                            for (std::list<FileAndFilter>::const_iterator
                                    it = nasm_headers.begin (),
                                    end = nasm_headers.end (); it != end; ++it) {
                                vcxprojfiltersFile << util::FormatString (
                                    VCXPROJ_FILTERS_HEADER_TEMPLATE,
                                    CreateRelativePath ((*it).first).c_str (),
                                    (*it).second.c_str ());
                            }
                        }
                        else if (variable == "nasm_sources") {
                            for (std::list<FileAndFilter>::const_iterator
                                    it = nasm_sources.begin (),
                                    end = nasm_sources.end (); it != end; ++it) {
                                vcxprojfiltersFile << util::FormatString (
                                    VCXPROJ_FILTERS_CUSTOM_BUILD_TEMPLATE,
                                    CreateRelativePath ((*it).first).c_str (),
                                    (*it).second.c_str ());
                            }
                        }
                        else if (variable == "c_headers") {
                            for (std::list<FileAndFilter>::const_iterator
                                    it = c_headers.begin (),
                                    end = c_headers.end (); it != end; ++it) {
                                vcxprojfiltersFile << util::FormatString (
                                    VCXPROJ_FILTERS_HEADER_TEMPLATE,
                                    CreateRelativePath ((*it).first).c_str (),
                                    (*it).second.c_str ());
                            }
                        }
                        else if (variable == "c_sources") {
                            for (std::list<FileAndFilter>::const_iterator
                                    it = c_sources.begin (),
                                    end = c_sources.end (); it != end; ++it) {
                                vcxprojfiltersFile << util::FormatString (
                                    VCXPROJ_FILTERS_SOURCE_TEMPLATE,
                                    CreateRelativePath ((*it).first).c_str (),
                                    (*it).second.c_str ());
                            }
                        }
                        else if (variable == "cpp_headers") {
                            for (std::list<FileAndFilter>::const_iterator
                                    it = cpp_headers.begin (),
                                    end = cpp_headers.end (); it != end; ++it) {
                                vcxprojfiltersFile << util::FormatString (
                                    VCXPROJ_FILTERS_HEADER_TEMPLATE,
                                    CreateRelativePath ((*it).first).c_str (),
                                    (*it).second.c_str ());
                            }
                        }
                        else if (variable == "cpp_sources") {
                            for (std::list<FileAndFilter>::const_iterator
                                    it = cpp_sources.begin (),
                                    end = cpp_sources.end (); it != end; ++it) {
                                vcxprojfiltersFile << util::FormatString (
                                    VCXPROJ_FILTERS_SOURCE_TEMPLATE,
                                    CreateRelativePath ((*it).first).c_str (),
                                    (*it).second.c_str ());
                            }
                        }
                        else if (variable == "rc_sources") {
                            for (std::list<FileAndFilter>::const_iterator
                                    it = rc_sources.begin (),
                                    end = rc_sources.end (); it != end; ++it) {
                                vcxprojfiltersFile << util::FormatString (
                                    VCXPROJ_FILTERS_RC_SOURCE_TEMPLATE,
                                    CreateRelativePath ((*it).first).c_str (),
                                    (*it).second.c_str ());
                            }
                        }
                        else if (variable == "resources") {
                            for (std::list<FileAndFilter>::const_iterator
                                    it = resources.begin (),
                                    end = resources.end (); it != end; ++it) {
                                vcxprojfiltersFile << util::FormatString (
                                    VCXPROJ_FILTERS_RESOURCE_TEMPLATE,
                                    CreateRelativePath ((*it).first).c_str (),
                                    (*it).second.c_str ());
                            }
                        }
                        else if (variable == "custom_build_sources") {
                            for (std::list<FileAndFilter>::const_iterator
                                    it = custom_build_sources.begin (),
                                    end = custom_build_sources.end (); it != end; ++it) {
                                vcxprojfiltersFile << (*it).second;
                            }
                        }
                        else {
                            vcxprojfiltersFile << ch << '(' << variable << ')';
                        }
                    }
                    else {
                        vcxprojfiltersFile << ch;
                    }
                }
            }
            else {
                THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                    "Unable to open '%s'.", vcxprojfiltersFilePath.size ());
            }
        }

        namespace {
            void GetLines (
                    const std::string &str,
                    std::vector<std::string> &lines) {
                std::string::size_type newLine;
                std::string::size_type start = 0;
                const char *NEW_LINE_CHARS = "\r\n";
                while ((newLine = str.find_first_of (NEW_LINE_CHARS, start)) != std::string::npos) {
                    // Line ending conventions.
                    // POSIX - '\n'
                    // OS X - '\r'
                    // Windows - '\r\n'
                    // If we're processing an '\r\n' file, remove both endings.
                    if (str[start] == '\r' && start < str.size () - 1 && str[start + 1] == '\n') {
                        ++start;
                    }
                    std::string line =
                        util::TrimRightSpaces (str.substr (start, newLine - start).c_str ());
                    if (!line.empty ()) {
                        lines.push_back (line);
                    }
                    start = newLine + 1;
                }
                std::string line = util::TrimRightSpaces (str.substr (start).c_str ());
                if (!line.empty ()) {
                    lines.push_back (line);
                }
            }

            std::string SaveRecipe (
                    const std::string &path,
                    const std::string &recipe) {
                if (!recipe.empty ()) {
                    util::Directory::Create (util::Path (ToSystemPath (path)).GetDirectory ());
                    std::fstream recipeFile (
                        ToSystemPath (path).c_str (),
                        std::fstream::out | std::fstream::binary | std::fstream::trunc);
                    std::vector<std::string> recipeLines;
                    GetLines (recipe, recipeLines);
                    for (std::size_t i = 0, count = recipeLines.size (); i < count; ++i) {
                        recipeFile << recipeLines[i] << "\n";
                    }
                    return util::FormatString (
                        "\"%s\" \"%s\"",
                        ToSystemPath (core::_TOOLCHAIN_SHELL).c_str (),
                        path.c_str ());
                }
                return std::string ();
            }
        }

        void vsBase::AddCustomBuildRule (
                const core::thekogans_make &thekogans_make,
                const core::thekogans_make::FileList::File &file,
                const std::string &prefix) {
            if (file.customBuild.get () != 0) {
                std::string outputs;
                {
                    std::size_t count = file.customBuild->outputs.size ();
                    if (count > 0) {
                        std::string prefix_ =
                            core::MakePath (
                                core::MakePath (
                                    thekogans_make.project_root,
                                    core::GetBuildDirectory (
                                        thekogans_make.generator,
                                        thekogans_make.config,
                                        thekogans_make.type)),
                                prefix);
                        outputs = ToSystemPath (core::MakePath (prefix_, file.customBuild->outputs[0]));
                        for (std::size_t i = 1; i < count; ++i) {
                            outputs += ";" + ToSystemPath (core::MakePath (prefix_, file.customBuild->outputs[i]));
                        }
                    }
                }
                std::string dependencies;
                {
                    std::size_t count = file.customBuild->dependencies.size ();
                    if (count > 0) {
                        dependencies = ToSystemPath (
                            core::MakePath (
                                thekogans_make.project_root,
                                file.customBuild->dependencies[0]));
                        for (std::size_t i = 1; i < count; ++i) {
                            dependencies += ";" + ToSystemPath (
                                core::MakePath (
                                    thekogans_make.project_root,
                                    file.customBuild->dependencies[i]));
                        }
                    }
                }
                custom_build_sources.push_back (
                    FileAndFilter (
                        util::FormatString (
                            VCXPROJ_CUSTOM_BUILD_TEMPLATE,
                            CreateRelativePath (
                                ToSystemPath (
                                    core::MakePath (prefix, file.name))).c_str (),
                            SaveRecipe (
                                core::MakePath (
                                    core::MakePath (
                                        core::MakePath (
                                            thekogans_make.project_root,
                                            core::GetBuildDirectory (
                                                thekogans_make.generator,
                                                thekogans_make.config,
                                                thekogans_make.type)),
                                        prefix),
                                    file.name + ".recipe"),
                                file.customBuild->recipe).c_str (),
                            !file.customBuild->message.empty () ?
                                file.customBuild->message.c_str () :
                                "Performing Custom Build Step on \"%(Identity)\"",
                            outputs.c_str (),
                            dependencies.c_str ()),
                        util::FormatString (
                            VCXPROJ_FILTERS_CUSTOM_BUILD_TEMPLATE,
                            CreateRelativePath (
                                ToSystemPath (
                                    core::MakePath (prefix, file.name))).c_str (),
                            GetFilter (VCXPROJ_FILTERS_CUSTOM_BUILD_SOURCES_FILTER_PREFIX,
                                file.name, custom_build_filters).c_str ())));
            }
        }

        std::string vsBase::GetFilter (
                const std::string &filterBase,
                const std::string &path,
                std::set<std::string> &filters) {
            std::string filter = filterBase;
            {
                std::list<std::string> components;
                util::Path (path).GetComponents (components);
                if (!components.empty ()) {
                    // Remove file name.
                    components.pop_back ();
                }
                // Whatever's left is the filter.
                filters.insert (filter);
                for (std::list<std::string>::const_iterator
                        it = components.begin (),
                        end = components.end (); it != end; ++it) {
                    filter += '\\';
                    filter += *it;
                    filters.insert (filter);
                }
            }
            return filter;
        }

        std::string vsBase::GetMasmCommandLine (const core::thekogans_make &thekogans_make) const {
            std::string masmCommandLine = "\"";
            masmCommandLine += GetMasmPath ();
            masmCommandLine += "\"";
            for (std::list<std::string>::const_iterator
                    it = thekogans_make.masm_flags.begin (),
                    end = thekogans_make.masm_flags.end (); it != end; ++it) {
                masmCommandLine += " ";
                masmCommandLine += *it;
            }
            std::list<std::string> preprocessorDefinitions;
            thekogans_make.GetCommonPreprocessorDefinitions (preprocessorDefinitions);
            for (std::list<std::string>::const_iterator
                    it = preprocessorDefinitions.begin (),
                    end = preprocessorDefinitions.end (); it != end; ++it) {
                masmCommandLine += " -D";
                masmCommandLine += *it;
            }
            for (std::list<std::string>::const_iterator
                    it = thekogans_make.masm_preprocessor_definitions.begin (),
                    end = thekogans_make.masm_preprocessor_definitions.end (); it != end; ++it) {
                masmCommandLine += " -D";
                masmCommandLine += *it;
            }
            std::set<std::string> include_directories;
            thekogans_make.GetIncludeDirectories (include_directories);
            for (std::set<std::string>::const_iterator
                    it = include_directories.begin (),
                    end = include_directories.end (); it != end; ++it) {
                masmCommandLine += " -I";
                masmCommandLine += ToSystemPath (*it);
            }
            masmCommandLine += " -Fo\"$(ProjectDir)$(IntDir)%(Filename).obj\" \"%(Identity)\"";
            return masmCommandLine;
        }

        std::string vsBase::GetNasmCommandLine (const core::thekogans_make &thekogans_make) const {
            std::string nasmCommandLine = "\"";
            nasmCommandLine += thekogans_make.Expand (GetNasmPath ().c_str ());
            nasmCommandLine += "\"";
            for (std::list<std::string>::const_iterator
                    it = thekogans_make.nasm_flags.begin (),
                    end = thekogans_make.nasm_flags.end (); it != end; ++it) {
                nasmCommandLine += " ";
                nasmCommandLine += *it;
            }
            std::list<std::string> preprocessorDefinitions;
            thekogans_make.GetCommonPreprocessorDefinitions (preprocessorDefinitions);
            for (std::list<std::string>::const_iterator
                    it = preprocessorDefinitions.begin (),
                    end = preprocessorDefinitions.end (); it != end; ++it) {
                nasmCommandLine += " -D";
                nasmCommandLine += *it;
            }
            for (std::list<std::string>::const_iterator
                    it = thekogans_make.nasm_preprocessor_definitions.begin (),
                    end = thekogans_make.nasm_preprocessor_definitions.end (); it != end; ++it) {
                nasmCommandLine += " -D";
                nasmCommandLine += *it;
            }
            std::set<std::string> include_directories;
            thekogans_make.GetIncludeDirectories (include_directories);
            for (std::set<std::string>::const_iterator
                    it = include_directories.begin (),
                    end = include_directories.end (); it != end; ++it) {
                nasmCommandLine += " -I";
                nasmCommandLine += ToSystemPath (*it);
            }
            nasmCommandLine += " -o \"$(ProjectDir)$(IntDir)%(Filename).obj\" \"%(Identity)\"";
            return nasmCommandLine;
        }

        std::string vsBase::GetRCPreprocessorDefinitions (
                const core::thekogans_make &thekogans_make) const {
            std::string rcPreprocessorDefinitions;
            std::list<std::string> preprocessorDefinitions;
            thekogans_make.GetCommonPreprocessorDefinitions (preprocessorDefinitions);
            for (std::list<std::string>::const_iterator
                    it = preprocessorDefinitions.begin (),
                    end = preprocessorDefinitions.end (); it != end; ++it) {
                rcPreprocessorDefinitions += *it;
                rcPreprocessorDefinitions += ';';
            }
            for (std::list<std::string>::const_iterator
                    it = thekogans_make.rc_preprocessor_definitions.begin (),
                    end = thekogans_make.rc_preprocessor_definitions.end (); it != end; ++it) {
                rcPreprocessorDefinitions += *it;
                rcPreprocessorDefinitions += ';';
            }
            return rcPreprocessorDefinitions;
        }

        std::string vsBase::GetRCIncludeDirectories (
                const core::thekogans_make &thekogans_make) const {
            std::string rcIncludeDirectories;
            {
                std::set<std::string> include_directories;
                thekogans_make.GetIncludeDirectories (include_directories);
                for (std::set<std::string>::const_iterator
                        it = include_directories.begin (),
                        end = include_directories.end (); it != end; ++it) {
                    rcIncludeDirectories += ToSystemPath (*it);
                    rcIncludeDirectories += ';';
                }
            }
            return rcIncludeDirectories;
        }

        std::string vsBase::GetQualifiedName (
                const std::string &organization,
                const std::string &project) const {
            return core::GetFileName (organization, project, std::string (), std::string (), std::string ());
        }

    } // namespace make
} // namespace thekogans
