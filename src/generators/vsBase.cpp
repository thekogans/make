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
#if defined (THEKOGANS_MAKE_CORE_HAVE_CURL)
    #include "thekogans/make/core/Sources.h"
#endif // defined (THEKOGANS_MAKE_CORE_HAVE_CURL)
#include "thekogans/make/generators/vsBase.h"

namespace thekogans {
    namespace make {
        namespace generators {

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
                    "      <Command>\"$(to_system_path -p:$(TOOLCHAIN_SHELL))\" \"$(TOOLCHAIN_ROOT)/common/bin/makebuild\" -g:$(generator) -pr:\"$(project_root)\" -c:$(config) -t:$(type)</Command>\n"
                    "    </PreBuildEvent>\n";
                const char * const VCXPROJ_POST_BUILD_EVENT_PLUGIN =
                    "    <PostBuildEvent>\n"
                    "      <Message>Copy plugin.</Message>\n"
                    "      <Command>\"$(to_system_path -p:$(TOOLCHAIN_SHELL))\" \"$(TOOLCHAIN_ROOT)/common/bin/copyplugin\" -pr:\"$(project_root)\" -c:$(config)</Command>\n"
                    "    </PostBuildEvent>\n";
                const char * const VCXPROJ_POST_BUILD_EVENT_PROGRAM =
                    "    <PostBuildEvent>\n"
                    "      <Message>Copy dependencies.</Message>\n"
                    "      <Command>\"$(to_system_path -p:$(TOOLCHAIN_SHELL))\" \"$(TOOLCHAIN_ROOT)/common/bin/copydependencies\" -pr:\"$(project_root)\" -c:$(config) -t:$(type)</Command>\n"
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

                inline std::string GetQualifiedName (
                        const std::string &organization,
                        const std::string &project) {
                    return core::GetFileName (organization, project, std::string (), std::string (), std::string ());
                }

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
                    // FIXME: This will not work with Flat naming convention.
                    // NOTE: Based on the design of the build folder:
                    // $(project_root)\build\$(TOOLCHAIN_BRANCH)\$(generator)\$(config)\$(type),
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
                        const std::vector<ProjectRootAndGUID> &projectdependencies,
                        const std::string &project_root) {
                    for (const auto &projectdependency : projectdependencies) {
                        if (projectdependency.project_root == project_root) {
                            return true;
                        }
                    }
                    return false;
                }

                void GetProjectDependencies (
                        const core::thekogans_make &thekogans_make,
                        std::vector<ProjectRootAndGUID> &projectDependencies,
                        bool recursive = true) {
                    for (auto dependency : thekogans_make.dependencies) {
                        if (dependency->GetConfigFile () == THEKOGANS_MAKE_XML &&
                                !FindProjectRoot (projectDependencies, dependency->GetProjectRoot ())) {
                            const core::thekogans_make &config = core::thekogans_make::GetConfig (
                                dependency->GetProjectRoot (),
                                dependency->GetConfigFile (),
                                dependency->GetGenerator (),
                                dependency->GetConfig (),
                                dependency->GetType ());
                            projectDependencies.push_back (
                                ProjectRootAndGUID (
                                    config.project_root,
                                    config.config,
                                    config.type,
                                    config.guid));
                            if (recursive) {
                                GetProjectDependencies (config, projectDependencies, recursive);
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
                const core::thekogans_make &thekogans_make =
                    core::thekogans_make::GetConfig (
                        project_root,
                        THEKOGANS_MAKE_XML,
                        Type (),
                        config,
                        type);
                if (rootProject) {
                    thekogans_make.CheckDependencies ();
                }
                std::string vcxprojFilePath =
                    ToSystemPath (
                        core::MakePath (
                            core::GetBuildRoot (project_root, Type (), config, type),
                            GetQualifiedName (
                                thekogans_make.organization,
                                thekogans_make.project) +
                            VCXPROJ_EXT));
                bool vcxprojFilePathExists = util::Path (vcxprojFilePath).Exists ();
                time_t vcxprojFilePathLastModifiedDate = 0;
                if (vcxprojFilePathExists) {
                    vcxprojFilePathLastModifiedDate =
                        util::Directory::Entry (vcxprojFilePath).lastModifiedDate;
                }
                bool updatedDependency = false;
                if (generateDependencies) {
                    if (thekogans_make.project_type == PROJECT_TYPE_PLUGIN) {
                        for (auto plugin_host : thekogans_make.plugin_hosts) {
                            if (plugin_host->GetConfigFile () == THEKOGANS_MAKE_XML) {
                                const core::thekogans_make &dependencyConfig =
                                    core::thekogans_make::GetConfig (
                                        plugin_host->GetProjectRoot (),
                                        plugin_host->GetConfigFile (),
                                        plugin_host->GetGenerator (),
                                        plugin_host->GetConfig (),
                                        plugin_host->GetType ());
                                vsBase::SharedPtr dependency =
                                    core::Generator::CreateGenerator (Type (), false);
                                updatedDependency |=
                                    dependency->Generate (
                                        plugin_host->GetProjectRoot (),
                                        plugin_host->GetConfig (),
                                        plugin_host->GetType (),
                                        generateDependencies,
                                        force) ||
                                    !vcxprojFilePathExists ||
                                    vcxprojFilePathLastModifiedDate <
                                        util::Directory::Entry (
                                            ToSystemPath (
                                                core::MakePath (
                                                    core::GetBuildRoot (
                                                        plugin_host->GetProjectRoot (),
                                                        Type (),
                                                        plugin_host->GetConfig (),
                                                        plugin_host->GetType ()),
                                                    GetQualifiedName (
                                                        dependencyConfig.organization,
                                                        dependencyConfig.project) +
                                                    VCXPROJ_EXT))).lastModifiedDate;
                            }
                        }
                    }
                    for (auto dependency : thekogans_make.dependencies) {
                        if (dependency->GetConfigFile () == THEKOGANS_MAKE_XML) {
                            const core::thekogans_make &dependencyConfig =
                                core::thekogans_make::GetConfig (
                                    dependency->GetProjectRoot (),
                                    dependency->GetConfigFile (),
                                    dependency->GetGenerator (),
                                    dependency->GetConfig (),
                                    dependency->GetType ());
                            vsBase::SharedPtr generator =
                                core::Generator::CreateGenerator (Type (), false);
                            updatedDependency |=
                                generator->Generate (
                                    dependency->GetProjectRoot (),
                                    dependency->GetConfig (),
                                    dependency->GetType (),
                                    generateDependencies,
                                    force) ||
                                !vcxprojFilePathExists ||
                                vcxprojFilePathLastModifiedDate <
                                    util::Directory::Entry (
                                        ToSystemPath (
                                            core::MakePath (
                                                core::GetBuildRoot (
                                                    dependency->GetProjectRoot (),
                                                    Type (),
                                                    dependency->GetConfig (),
                                                    dependency->GetType ()),
                                                GetQualifiedName (
                                                    dependencyConfig.organization,
                                                    dependencyConfig.project) +
                                                VCXPROJ_EXT))).lastModifiedDate;
                        }
                    }
                }
                std::string thekogans_makeFilePath =
                    ToSystemPath (core::MakePath (project_root, THEKOGANS_MAKE_XML));
                if (force ||
                        updatedDependency ||
                        !vcxprojFilePathExists ||
                    #if defined (THEKOGANS_MAKE_CORE_HAVE_CURL)
                        vcxprojFilePathLastModifiedDate <
                            util::Directory::Entry (
                                ToSystemPath (
                                    core::MakePath (
                                        core::_TOOLCHAIN_ROOT, SOURCES_XML))).lastModifiedDate ||
                    #endif // defined (THEKOGANS_MAKE_CORE_HAVE_CURL)
                        vcxprojFilePathLastModifiedDate <
                            util::Directory::Entry (thekogans_makeFilePath).lastModifiedDate) {
                    for (auto masm_header : thekogans_make.masm_headers) {
                        for (auto file : masm_header->files) {
                            if (file->customBuild != nullptr) {
                                AddCustomBuildRule (thekogans_make, *file, masm_header->prefix);
                            }
                            else {
                                masm_headers.push_back (
                                    FileAndFilter (
                                        ToSystemPath (core::MakePath (masm_header->prefix, file->name)),
                                        GetFilter (VCXPROJ_FILTERS_MASM_HEADERS_FILTER_PREFIX,
                                            file->name, header_filters)));
                            }
                        }
                    }
                    for (auto masm_source : thekogans_make.masm_sources) {
                        for (auto file : masm_source->files) {
                            if (file->customBuild != nullptr) {
                                AddCustomBuildRule (thekogans_make, *file, masm_source->prefix);
                            }
                            else {
                                masm_sources.push_back (
                                    FileAndFilter (
                                        ToSystemPath (core::MakePath (masm_source->prefix, file->name)),
                                        GetFilter (VCXPROJ_FILTERS_MASM_SOURCES_FILTER_PREFIX,
                                            file->name, source_filters)));
                            }
                        }
                    }
                    for (auto nasm_header : thekogans_make.nasm_headers) {
                        for (auto file : nasm_header->files) {
                            if (file->customBuild != nullptr) {
                                AddCustomBuildRule (thekogans_make, *file, nasm_header->prefix);
                            }
                            else {
                                nasm_headers.push_back (
                                    FileAndFilter (
                                        ToSystemPath (core::MakePath (nasm_header->prefix, file->name)),
                                        GetFilter (VCXPROJ_FILTERS_NASM_HEADERS_FILTER_PREFIX,
                                            file->name, header_filters)));
                            }
                        }
                    }
                    for (auto nasm_source : thekogans_make.nasm_sources) {
                        for (auto file : nasm_source->files) {
                            if (file->customBuild != nullptr) {
                                AddCustomBuildRule (thekogans_make, *file, nasm_source->prefix);
                            }
                            else {
                                nasm_sources.push_back (
                                    FileAndFilter (
                                        ToSystemPath (core::MakePath (nasm_source->prefix, file->name)),
                                        GetFilter (VCXPROJ_FILTERS_NASM_SOURCES_FILTER_PREFIX,
                                            file->name, source_filters)));
                            }
                        }
                    }
                    for (auto c_header : thekogans_make.c_headers) {
                        for (auto file : c_header->files) {
                            if (file->customBuild != nullptr) {
                                AddCustomBuildRule (thekogans_make, *file, c_header->prefix);
                            }
                            else {
                                c_headers.push_back (
                                    FileAndFilter (
                                        ToSystemPath (core::MakePath (c_header->prefix, file->name)),
                                        GetFilter (VCXPROJ_FILTERS_C_HEADERS_FILTER_PREFIX,
                                            file->name, header_filters)));
                            }
                        }
                    }
                    for (auto c_source : thekogans_make.c_sources) {
                        for (auto file : c_source->files) {
                            if (file->customBuild != nullptr) {
                                AddCustomBuildRule (thekogans_make, *file, c_source->prefix);
                            }
                            else {
                                c_sources.push_back (
                                    FileAndFilter (
                                        ToSystemPath (core::MakePath (c_source->prefix, file->name)),
                                        GetFilter (VCXPROJ_FILTERS_C_SOURCES_FILTER_PREFIX,
                                            file->name, source_filters)));
                            }
                        }
                    }
                    for (auto cpp_header : thekogans_make.cpp_headers) {
                        for (auto file : cpp_header->files) {
                            if (file->customBuild != nullptr) {
                                AddCustomBuildRule (thekogans_make, *file, cpp_header->prefix);
                            }
                            else {
                                cpp_headers.push_back (
                                    FileAndFilter (
                                        ToSystemPath (core::MakePath (cpp_header->prefix, file->name)),
                                        GetFilter (VCXPROJ_FILTERS_CPP_HEADERS_FILTER_PREFIX,
                                            file->name, header_filters)));
                            }
                        }
                    }
                    for (auto cpp_source : thekogans_make.cpp_sources) {
                        for (auto file : cpp_source->files) {
                            if (file->customBuild != nullptr) {
                                AddCustomBuildRule (thekogans_make, *file, cpp_source->prefix);
                            }
                            else {
                                cpp_sources.push_back (
                                    FileAndFilter (
                                        ToSystemPath (core::MakePath (cpp_source->prefix, file->name)),
                                        GetFilter (VCXPROJ_FILTERS_CPP_SOURCES_FILTER_PREFIX,
                                            file->name, source_filters)));
                            }
                        }
                    }
                    for (auto rc_source : thekogans_make.rc_sources) {
                        for (auto file : rc_source->files) {
                            if (file->customBuild != nullptr) {
                                AddCustomBuildRule (thekogans_make, *file, rc_source->prefix);
                            }
                            else {
                                rc_sources.push_back (
                                    FileAndFilter (
                                        ToSystemPath (core::MakePath (rc_source->prefix, file->name)),
                                        GetFilter (VCXPROJ_FILTERS_RC_SOURCES_FILTER_PREFIX,
                                            file->name, rc_source_filters)));
                            }
                        }
                    }
                    for (auto resource : thekogans_make.resources) {
                        for (auto file : resource->files) {
                            if (file->customBuild != nullptr) {
                                AddCustomBuildRule (thekogans_make, *file, resource->prefix);
                            }
                            else {
                                resources.push_back (
                                    FileAndFilter (
                                        ToSystemPath (core::MakePath (resource->prefix, file->name)),
                                        GetFilter (VCXPROJ_FILTERS_RESOURCES_FILTER_PREFIX,
                                            file->name, resource_filters)));
                            }
                        }
                    }
                    core::CreateBuildRoot (
                        thekogans_make.project_root,
                        Type (),
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
                        Type (),
                        config,
                        type);
                if (deleteDependencies) {
                    for (auto dependency : thekogans_make.dependencies) {
                        if (dependency->GetConfigFile () == THEKOGANS_MAKE_XML) {
                            vsBase::SharedPtr generator =
                                core::Generator::CreateGenerator (Type (), false);
                            generator->Delete (
                                dependency->GetProjectRoot (),
                                dependency->GetConfig (),
                                dependency->GetType (),
                                deleteDependencies);
                        }
                    }
                }
                std::string build_root = core::GetBuildRoot (project_root, Type (), config, type);
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
                            Type (),
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
                            Type (),
                            thekogans_make.config,
                            thekogans_make.type),
                        GetQualifiedName (
                            thekogans_make.organization,
                            thekogans_make.project) + SLN_EXT);
                std::fstream slnFile (
                    ToSystemPath (slnFilePath).c_str (),
                    std::fstream::out | std::fstream::binary | std::fstream::trunc);
                if (slnFile.is_open ()) {
                    std::vector<ProjectRootAndGUID> projectDependencies;
                    GetProjectDependencies (thekogans_make, projectDependencies);
                    const char *fileTemplate = slnTemplate;
                    while (*fileTemplate != '\0') {
                        char ch = *fileTemplate++;
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
                                for (const auto &projectDependency : projectDependencies) {
                                    const core::thekogans_make &dependency = core::thekogans_make::GetConfig (
                                        projectDependency.project_root,
                                        THEKOGANS_MAKE_XML,
                                        Type (),
                                        projectDependency.config,
                                        projectDependency.type);
                                    slnFile << util::FormatString (
                                        SLN_PROJECT_TEMPLATE,
                                        GetQualifiedName (dependency.organization, dependency.project).c_str (),
                                        ToSystemPath (
                                            core::MakePath (
                                                core::GetBuildRoot (
                                                    dependency.project_root,
                                                    Type (),
                                                    dependency.config,
                                                    dependency.type),
                                                GetQualifiedName (
                                                    dependency.organization,
                                                    dependency.project) + VCXPROJ_EXT)).c_str (),
                                        dependency.guid.ToHexString (true).c_str ());
                                    std::vector<ProjectRootAndGUID> dependencyDependencies;
                                    GetProjectDependencies (dependency, dependencyDependencies, false);
                                    if (!dependencyDependencies.empty ()) {
                                        slnFile << SLN_PROJECT_SECTION;
                                        for (const auto &dependencyDependency : dependencyDependencies) {
                                            std::string guidString = dependencyDependency.guid.ToHexString (true);
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
                                                Type (),
                                                thekogans_make.config,
                                                thekogans_make.type),
                                            GetQualifiedName (
                                                thekogans_make.organization,
                                                thekogans_make.project) + VCXPROJ_EXT)).c_str (),
                                    thekogans_make.guid.ToHexString (true).c_str ());
                                std::vector<ProjectRootAndGUID> projectDependencies;
                                GetProjectDependencies (thekogans_make, projectDependencies, false);
                                if (!projectDependencies.empty ()) {
                                    slnFile << SLN_PROJECT_SECTION;
                                    for (const auto &projectDependency : projectDependencies) {
                                        std::string guidString = projectDependency.guid.ToHexString (true);
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
                                for (const auto &projectDependency : projectDependencies) {
                                    std::string guidString = projectDependency.guid.ToHexString (true);
                                    slnFile << thekogans_make.Expand (
                                        util::FormatString (
                                            GetSLN_DEPENDENCY_TARGET_TEMPLATE (),
                                            guidString.c_str (),
                                            guidString.c_str ()).c_str ());
                                }
                            }
                            else if (variable == "project_targets") {
                                std::string guidString = thekogans_make.guid.ToHexString (true);
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
                    "      <PreprocessorDefinitions>$(preprocessor_definitions);%(PreprocessorDefinitions)</PreprocessorDefinitions>\n"
                    "      <RuntimeLibrary>$(runtime_library)</RuntimeLibrary>\n"
                    "      <DebugInformationFormat>$(debug_information_format)</DebugInformationFormat>\n"
                    "      <MinimalRebuild>false</MinimalRebuild>\n"
                    "      <MultiProcessorCompilation>true</MultiProcessorCompilation>\n"
                    "    </ClCompile>\n"
                    "    <Link>\n"
                    "      <TargetMachine>$(target_machine)</TargetMachine>\n"
                    "      <GenerateDebugInformation>$(generate_debug_information)</GenerateDebugInformation>\n"
                    "      <AdditionalDependencies>$(link_libraries)%(AdditionalDependencies)</AdditionalDependencies>\n"
                    "$(import_library)\n"
                    "$(module_definition_file)\n"
                    "$(sub_system)\n"
                    "      <ForceFileOutput>MultiplyDefinedSymbolOnly</ForceFileOutput>"
                    "    </Link>\n"
                    "    <Lib>\n"
                    "      <TargetMachine>$(target_machine)</TargetMachine>\n"
                    "    </Lib>\n"
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
                            Type (),
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
                            Type (),
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
                        char ch = *fileTemplate++;
                        if (ch == '$') {
                            std::string variable = GetVariable (&fileTemplate);
                            if (variable == "tools_version") {
                                vcxprojFile << vcxprojGetToolsVersion ();
                            }
                            else if (variable == "platform_toolset") {
                                vcxprojFile << vcxprojGetPlatformToolset ();
                            }
                            else if (variable == "project_guid") {
                                vcxprojFile << thekogans_make.guid.ToHexString (true);
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
                                vcxprojFile << Type ();
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
                                for (const auto &include_directory : include_directories) {
                                    vcxprojFile << ToSystemPath (include_directory) << ';';
                                }
                            }
                            else if (variable == "preprocessor_definitions") {
                                if (thekogans_make.project_type == PROJECT_TYPE_PROGRAM) {
                                    vcxprojFile << (thekogans_make.subsystem == "Console" ? "_CONSOLE;" : "_WINDOWS;");
                                }
                                if (core::_TOOLCHAIN_ARCH == ARCH_i386) {
                                    vcxprojFile << "WIN32;";
                                }
                                else if (core::_TOOLCHAIN_ARCH == ARCH_x86_64) {
                                    vcxprojFile << "WIN64;";
                                }
                                if (thekogans_make.project_type == PROJECT_TYPE_LIBRARY &&
                                        thekogans_make.type == TYPE_STATIC) {
                                    vcxprojFile << "_LIB;";
                                }
                                if (thekogans_make.config == CONFIG_DEBUG) {
                                    vcxprojFile << "_DEBUG;";
                                }
                                else if (thekogans_make.config == CONFIG_RELEASE) {
                                    vcxprojFile << "NDEBUG;";
                                }
                                vcxprojFile << "BOOST_ALL_NO_LIB;";
                                std::set<std::string> preprocessor_definitions;
                                thekogans_make.GetCommonPreprocessorDefinitions (preprocessor_definitions);
                                for (const auto &preprocessor_definition : preprocessor_definitions) {
                                    vcxprojFile << preprocessor_definition << ';';
                                }
                                for (const auto &preprocessor_definition : thekogans_make.preprocessor_definitions) {
                                    vcxprojFile << preprocessor_definition << ';';
                                }
                                for (const auto &preprocessor_definition : thekogans_make.c_preprocessor_definitions) {
                                    vcxprojFile << preprocessor_definition << ';';
                                }
                                for (const auto &preprocessor_definition : thekogans_make.cpp_preprocessor_definitions) {
                                    vcxprojFile << preprocessor_definition << ';';
                                }
                                std::set<std::string> features;
                                thekogans_make.GetFeatures (features);
                                for (const auto &feature :features) {
                                    vcxprojFile << feature << ';';
                                }
                            }
                            else if (variable == "runtime_library") {
                                vcxprojFile << GetRuntimeLibrary (thekogans_make.config, thekogans_make.type);
                            }
                            else if (variable == "link_libraries") {
                                std::set<std::string> link_libraries;
                                thekogans_make.GetLinkLibraries (link_libraries);
                                for (const auto &link_library : link_libraries) {
                                    vcxprojFile << ToSystemPath (link_library) << ';';
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
                                for (const auto &masm_header :  masm_headers) {
                                    vcxprojFile << util::FormatString (
                                        VCXPROJ_HEADER_TEMPLATE,
                                        CreateRelativePath (masm_header.first).c_str ());
                                }
                            }
                            else if (variable == "masm_sources") {
                                for (const auto &masm_source :  masm_sources) {
                                    vcxprojFile << util::FormatString (
                                        VCXPROJ_CUSTOM_BUILD_TEMPLATE,
                                        CreateRelativePath (masm_source.first).c_str (),
                                        GetMasmCommandLine (thekogans_make).c_str (),
                                        "Performing Custom Build Step on \"%(Identity)\"",
                                        "$(ProjectDir)$(IntDir)%(Filename).obj", "");
                                }
                            }
                            else if (variable == "nasm_headers") {
                                for (const auto &nasm_header : nasm_headers) {
                                    vcxprojFile << util::FormatString (
                                        VCXPROJ_HEADER_TEMPLATE,
                                        CreateRelativePath (nasm_header.first).c_str ());
                                }
                            }
                            else if (variable == "nasm_sources") {
                                for (const auto &nasm_source :  nasm_sources) {
                                    vcxprojFile << util::FormatString (
                                        VCXPROJ_CUSTOM_BUILD_TEMPLATE,
                                        CreateRelativePath (nasm_source.first).c_str (),
                                        GetNasmCommandLine (thekogans_make).c_str (),
                                        "Performing Custom Build Step on \"%(Identity)\"",
                                        "$(ProjectDir)$(IntDir)%(Filename).obj", "");
                                }
                            }
                            else if (variable == "c_headers") {
                                for (const auto &c_header : c_headers) {
                                    vcxprojFile << util::FormatString (
                                        VCXPROJ_HEADER_TEMPLATE,
                                        CreateRelativePath (c_header.first).c_str ());
                                }
                            }
                            else if (variable == "c_sources") {
                                for (const auto &c_source : c_sources) {
                                    vcxprojFile << util::FormatString (
                                        VCXPROJ_SOURCE_TEMPLATE,
                                        CreateRelativePath (c_source.first).c_str (),
                                        util::Path (c_source.first).GetDirectory (true).c_str ());
                                }
                            }
                            else if (variable == "cpp_headers") {
                                for (const auto &cpp_header : cpp_headers) {
                                    vcxprojFile << util::FormatString (
                                        VCXPROJ_HEADER_TEMPLATE,
                                        CreateRelativePath (cpp_header.first).c_str ());
                                }
                            }
                            else if (variable == "cpp_sources") {
                                for (const auto &cpp_source : cpp_sources) {
                                    vcxprojFile << util::FormatString (
                                        VCXPROJ_SOURCE_TEMPLATE,
                                        CreateRelativePath (cpp_source.first).c_str (),
                                        util::Path (cpp_source.first).GetDirectory (true).c_str ());
                                }
                            }
                            else if (variable == "rc_sources") {
                                for (const auto &rc_source : rc_sources) {
                                    vcxprojFile << util::FormatString (
                                        VCXPROJ_RC_SOURCE_TEMPLATE,
                                        CreateRelativePath (rc_source.first).c_str (),
                                        GetRCPreprocessorDefinitions (thekogans_make).c_str (),
                                        GetRCIncludeDirectories (thekogans_make).c_str ());
                                }
                            }
                            else if (variable == "resources") {
                                for (const auto &resource : resources) {
                                    vcxprojFile << util::FormatString (
                                        VCXPROJ_RESOURCE_TEMPLATE,
                                        CreateRelativePath (resource.first).c_str ());
                                }
                            }
                            else if (variable == "custom_build_sources") {
                                for (const auto &custom_build_source : custom_build_sources) {
                                    vcxprojFile << custom_build_source.first;
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
                            Type (),
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
                            Type (),
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
                        char ch = *fileTemplate++;
                        if (ch == '$') {
                            std::string variable = GetVariable (&fileTemplate);
                            if (variable == "tools_version") {
                                vcxprojfiltersFile << vcxprojfiltersGetToolsVersion ();
                            }
                            else if (variable == "filters") {
                                for (const auto &header_filter : header_filters) {
                                    vcxprojfiltersFile << util::FormatString (
                                        VCXPROJ_FILTERS_FILTER_TEMPLATE,
                                        header_filter.c_str (),
                                        util::GUID::FromRandom ().ToHexString (true).c_str ());
                                }
                                for (const auto &source_filter : source_filters) {
                                    vcxprojfiltersFile << util::FormatString (
                                        VCXPROJ_FILTERS_FILTER_TEMPLATE,
                                        source_filter.c_str (),
                                        util::GUID::FromRandom ().ToHexString (true).c_str ());
                                }
                                for (const auto &rc_source_filter : rc_source_filters) {
                                    vcxprojfiltersFile << util::FormatString (
                                        VCXPROJ_FILTERS_FILTER_TEMPLATE,
                                        rc_source_filter.c_str (),
                                        util::GUID::FromRandom ().ToHexString (true).c_str ());
                                }
                                for (const auto &resource_filter : resource_filters) {
                                    vcxprojfiltersFile << util::FormatString (
                                        VCXPROJ_FILTERS_FILTER_TEMPLATE,
                                        resource_filter.c_str (),
                                        util::GUID::FromRandom ().ToHexString (true).c_str ());
                                }
                                for (const auto &custom_build_filter : custom_build_filters) {
                                    vcxprojfiltersFile << util::FormatString (
                                        VCXPROJ_FILTERS_FILTER_TEMPLATE,
                                        custom_build_filter.c_str (),
                                        util::GUID::FromRandom ().ToHexString (true).c_str ());
                                }
                            }
                            else if (variable == "masm_headers") {
                                for (const auto &masm_header : masm_headers) {
                                    vcxprojfiltersFile << util::FormatString (
                                        VCXPROJ_FILTERS_HEADER_TEMPLATE,
                                        CreateRelativePath (masm_header.first).c_str (),
                                        masm_header.second.c_str ());
                                }
                            }
                            else if (variable == "masm_sources") {
                                for (const auto &masm_source : masm_sources) {
                                    vcxprojfiltersFile << util::FormatString (
                                        VCXPROJ_FILTERS_CUSTOM_BUILD_TEMPLATE,
                                        CreateRelativePath (masm_source.first).c_str (),
                                        masm_source.second.c_str ());
                                }
                            }
                            else if (variable == "nasm_headers") {
                                for (const auto &nasm_header : nasm_headers) {
                                    vcxprojfiltersFile << util::FormatString (
                                        VCXPROJ_FILTERS_HEADER_TEMPLATE,
                                        CreateRelativePath (nasm_header.first).c_str (),
                                        nasm_header.second.c_str ());
                                }
                            }
                            else if (variable == "nasm_sources") {
                                for (const auto &nasm_source : nasm_sources) {
                                    vcxprojfiltersFile << util::FormatString (
                                        VCXPROJ_FILTERS_CUSTOM_BUILD_TEMPLATE,
                                        CreateRelativePath (nasm_source.first).c_str (),
                                        nasm_source.second.c_str ());
                                }
                            }
                            else if (variable == "c_headers") {
                                for (const auto &c_header :c_headers) {
                                    vcxprojfiltersFile << util::FormatString (
                                        VCXPROJ_FILTERS_HEADER_TEMPLATE,
                                        CreateRelativePath (c_header.first).c_str (),
                                        c_header.second.c_str ());
                                }
                            }
                            else if (variable == "c_sources") {
                                for (const auto &c_source : c_sources) {
                                    vcxprojfiltersFile << util::FormatString (
                                        VCXPROJ_FILTERS_SOURCE_TEMPLATE,
                                        CreateRelativePath (c_source.first).c_str (),
                                        c_source.second.c_str ());
                                }
                            }
                            else if (variable == "cpp_headers") {
                                for (const auto &cpp_header : cpp_headers) {
                                    vcxprojfiltersFile << util::FormatString (
                                        VCXPROJ_FILTERS_HEADER_TEMPLATE,
                                        CreateRelativePath (cpp_header.first).c_str (),
                                        cpp_header.second.c_str ());
                                }
                            }
                            else if (variable == "cpp_sources") {
                                for (const auto &cpp_source : cpp_sources) {
                                    vcxprojfiltersFile << util::FormatString (
                                        VCXPROJ_FILTERS_SOURCE_TEMPLATE,
                                        CreateRelativePath (cpp_source.first).c_str (),
                                        cpp_source.second.c_str ());
                                }
                            }
                            else if (variable == "rc_sources") {
                                for (const auto &rc_source : rc_sources) {
                                    vcxprojfiltersFile << util::FormatString (
                                        VCXPROJ_FILTERS_RC_SOURCE_TEMPLATE,
                                        CreateRelativePath (rc_source.first).c_str (),
                                        rc_source.second.c_str ());
                                }
                            }
                            else if (variable == "resources") {
                                for (const auto &resource : resources) {
                                    vcxprojfiltersFile << util::FormatString (
                                        VCXPROJ_FILTERS_RESOURCE_TEMPLATE,
                                        CreateRelativePath (resource.first).c_str (),
                                        resource.second.c_str ());
                                }
                            }
                            else if (variable == "custom_build_sources") {
                                for (const auto &custom_build_source : custom_build_sources) {
                                    vcxprojfiltersFile << custom_build_source.second;
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
                        for (const auto &recipeLine : recipeLines) {
                            recipeFile << recipeLine << "\n";
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
                if (file.customBuild != nullptr) {
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
                    std::vector<std::string> components;
                    util::Path (path).GetComponents (components);
                    if (!components.empty ()) {
                        // Remove file name.
                        components.pop_back ();
                    }
                    // Whatever's left is the filter.
                    filters.insert (filter);
                    for (const auto &component : components) {
                        filter += '\\';
                        filter += component;
                        filters.insert (filter);
                    }
                }
                return filter;
            }

            std::string vsBase::GetMasmCommandLine (const core::thekogans_make &thekogans_make) const {
                std::string masmCommandLine = "\"";
                masmCommandLine += GetMasmPath ();
                masmCommandLine += "\"";
                for (const auto &masm_flag : thekogans_make.masm_flags) {
                    masmCommandLine += " ";
                    masmCommandLine += masm_flag;
                }
                std::set<std::string> preprocessorDefinitions;
                thekogans_make.GetCommonPreprocessorDefinitions (preprocessorDefinitions);
                for (const auto &preprocessorDefinition : preprocessorDefinitions) {
                    masmCommandLine += " -D";
                    masmCommandLine += preprocessorDefinition;
                }
                for (const auto &masm_preprocessor_definition : thekogans_make.masm_preprocessor_definitions) {
                    masmCommandLine += " -D";
                    masmCommandLine += masm_preprocessor_definition;
                }
                std::set<std::string> include_directories;
                thekogans_make.GetIncludeDirectories (include_directories);
                for (const auto &include_directory : include_directories) {
                    masmCommandLine += " -I";
                    masmCommandLine += ToSystemPath (include_directory);
                }
                masmCommandLine += " -Fo\"$(ProjectDir)$(IntDir)%(Filename).obj\" \"%(Identity)\"";
                return masmCommandLine;
            }

            std::string vsBase::GetNasmCommandLine (const core::thekogans_make &thekogans_make) const {
                std::string nasmCommandLine = "\"";
                nasmCommandLine += thekogans_make.Expand (GetNasmPath ().c_str ());
                nasmCommandLine += "\"";
                for (const auto &nasm_flag : thekogans_make.nasm_flags) {
                    nasmCommandLine += " ";
                    nasmCommandLine += nasm_flag;
                }
                std::set<std::string> preprocessorDefinitions;
                thekogans_make.GetCommonPreprocessorDefinitions (preprocessorDefinitions);
                for (const auto &preprocessorDefinition : preprocessorDefinitions) {
                    nasmCommandLine += " -D";
                    nasmCommandLine += preprocessorDefinition;
                }
                for (const auto &nasm_preprocessor_definition : thekogans_make.nasm_preprocessor_definitions) {
                    nasmCommandLine += " -D";
                    nasmCommandLine += nasm_preprocessor_definition;
                }
                std::set<std::string> include_directories;
                thekogans_make.GetIncludeDirectories (include_directories);
                for (const auto &include_directory : include_directories) {
                    nasmCommandLine += " -I";
                    nasmCommandLine += ToSystemPath (include_directory);
                }
                nasmCommandLine += " -o \"$(ProjectDir)$(IntDir)%(Filename).obj\" \"%(Identity)\"";
                return nasmCommandLine;
            }

            std::string vsBase::GetRCPreprocessorDefinitions (
                    const core::thekogans_make &thekogans_make) const {
                std::string rcPreprocessorDefinitions;
                std::set<std::string> preprocessorDefinitions;
                thekogans_make.GetCommonPreprocessorDefinitions (preprocessorDefinitions);
                for (const auto &preprocessorDefinition : preprocessorDefinitions) {
                    rcPreprocessorDefinitions += preprocessorDefinition;
                    rcPreprocessorDefinitions += ';';
                }
                for (const auto &rc_preprocessor_definition : thekogans_make.rc_preprocessor_definitions) {
                    rcPreprocessorDefinitions += rc_preprocessor_definition;
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
                    for (const auto &include_directory : include_directories) {
                        rcIncludeDirectories += ToSystemPath (include_directory);
                        rcIncludeDirectories += ';';
                    }
                }
                return rcIncludeDirectories;
            }

        } // namespace generators
    } // namespace make
} // namespace thekogans
