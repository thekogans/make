// Copyright 2016 Boris Kogan (boris@thekogans.net)
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

#if defined (THEKOGANS_MAKE_TYPE_Static)
    #include "thekogans/util/Environment.h"
    #include "thekogans/make/actions/build_project.h"
    #include "thekogans/make/actions/check_dependencies.h"
    #include "thekogans/make/actions/cleanup_toolchain.h"
    #include "thekogans/make/actions/copy_dependencies.h"
    #include "thekogans/make/actions/copy_plugin.h"
    #include "thekogans/make/actions/create_build_system.h"
    #include "thekogans/make/actions/delete_build_system.h"
    #include "thekogans/make/actions/get_build_config.h"
    #include "thekogans/make/actions/get_build_type.h"
    #include "thekogans/make/actions/get_naming_convention.h"
    #include "thekogans/make/actions/get_project_dependency_version.h"
    #include "thekogans/make/actions/get_project_type.h"
    #include "thekogans/make/actions/get_project_version.h"
    #include "thekogans/make/actions/get_schema_version.h"
    #include "thekogans/make/actions/get_toolchain_dependency_version.h"
    #include "thekogans/make/actions/get_toolchain_versions.h"
    #include "thekogans/make/actions/install_library.h"
    #include "thekogans/make/actions/install_plugin_hosts.h"
    #include "thekogans/make/actions/install_plugin.h"
    #include "thekogans/make/actions/install_program.h"
    #include "thekogans/make/actions/list_dependencies.h"
    #include "thekogans/make/actions/list_toolchain_sources.h"
    #include "thekogans/make/actions/uninstall_library.h"
    #include "thekogans/make/actions/uninstall_plugin.h"
    #include "thekogans/make/actions/uninstall_program.h"
#if defined (THEKOGANS_MAKE_HAVE_CURL)
    #include "thekogans/make/actions/add_source_project.h"
    #include "thekogans/make/actions/add_source_toolchain.h"
    #include "thekogans/make/actions/add_toolchain_source.h"
    #include "thekogans/make/actions/cleanup_source_project.h"
    #include "thekogans/make/actions/cleanup_source_toolchain.h"
    #include "thekogans/make/actions/cleanup_source.h"
    #include "thekogans/make/actions/create_source.h"
    #include "thekogans/make/actions/delete_source_project.h"
    #include "thekogans/make/actions/delete_source_toolchain.h"
    #include "thekogans/make/actions/delete_toolchain_source.h"
    #include "thekogans/make/actions/destroy_source.h"
    #include "thekogans/make/actions/get_source_project_latest_version.h"
    #include "thekogans/make/actions/get_source_project_sha2_256.h"
    #include "thekogans/make/actions/get_source_project_versions.h"
    #include "thekogans/make/actions/get_source_toolchain_file.h"
    #include "thekogans/make/actions/get_source_toolchain_latest_version.h"
    #include "thekogans/make/actions/get_source_toolchain_sha2_256.h"
    #include "thekogans/make/actions/get_source_toolchain_versions.h"
    #include "thekogans/make/actions/get_source_url.h"
    #include "thekogans/make/actions/get_toolchain_source_project_latest_version.h"
    #include "thekogans/make/actions/get_toolchain_source_project_sha2_256.h"
    #include "thekogans/make/actions/get_toolchain_source_project_versions.h"
    #include "thekogans/make/actions/get_toolchain_source_toolchain_file.h"
    #include "thekogans/make/actions/get_toolchain_source_toolchain_latest_version.h"
    #include "thekogans/make/actions/get_toolchain_source_toolchain_sha2_256.h"
    #include "thekogans/make/actions/get_toolchain_source_toolchain_versions.h"
    #include "thekogans/make/actions/get_toolchain_source_url.h"
    #include "thekogans/make/actions/update_toolchain_sources.h"
#endif // defined (THEKOGANS_MAKE_HAVE_CURL)
#if defined (TOOLCHAIN_OS_Windows)
    #include "thekogans/make/actions/list_cygwin_mount_table.h"
    #include "thekogans/make/actions/update_cl_dependencies.h"
    #include "thekogans/make/actions/update_windows_dependencies.h"
#endif // defined (TOOLCHAIN_OS_Windows)
    #include "thekogans/make/actions/Actions.h"
#endif // defined (THEKOGANS_MAKE_TYPE_Static)

namespace thekogans {
    namespace make {
        namespace actions {

        #if defined (THEKOGANS_MAKE_TYPE_Static)
            void Actions::StaticInit () {
                build_project::StaticInit ();
                check_dependencies::StaticInit ();
                cleanup_toolchain::StaticInit ();
                copy_dependencies::StaticInit ();
                copy_plugin::StaticInit ();
                create_build_system::StaticInit ();
                delete_build_system::StaticInit ();
                get_build_config::StaticInit ();
                get_build_type::StaticInit ();
                get_naming_convention::StaticInit ();
                project_dependency_version::StaticInit ();
                get_project_type::StaticInit ();
                get_project_version::StaticInit ();
                get_schema_version::StaticInit ();
                get_toolchain_dependency_version::StaticInit ();
                get_toolchain_versions::StaticInit ();
                install_library::StaticInit ();
                install_plugin_hosts::StaticInit ();
                install_plugin::StaticInit ();
                install_program::StaticInit ();
                list_dependencies::StaticInit ();
                list_toolchain_sources::StaticInit ();
                uninstall_library::StaticInit ();
                uninstall_plugin::StaticInit ();
                uninstall_program::StaticInit ();
            #if defined (THEKOGANS_MAKE_HAVE_CURL)
                add_source_project::StaticInit ();
                add_source_toolchain::StaticInit ();
                add_toolchain_source::StaticInit ();
                cleanup_source_project::StaticInit ();
                cleanup_source_toolchain::StaticInit ();
                cleanup_source::StaticInit ();
                create_source::StaticInit ();
                delete_source_project::StaticInit ();
                delete_source_toolchain::StaticInit ();
                delete_toolchain_source::StaticInit ();
                destroy_source::StaticInit ();
                get_source_project_latest_version::StaticInit ();
                get_source_project_sha2_256::StaticInit ();
                get_source_project_versions::StaticInit ();
                get_source_toolchain_file::StaticInit ();
                get_source_toolchain_latest_version::StaticInit ();
                get_source_toolchain_sha2_256::StaticInit ();
                get_source_toolchain_versions::StaticInit ();
                get_source_url::StaticInit ();
                get_toolchain_source_project_latest_version::StaticInit ();
                get_toolchain_source_project_sha2_256::StaticInit ();
                get_toolchain_source_project_versions::StaticInit ();
                get_toolchain_source_toolchain_file::StaticInit ();
                get_toolchain_source_toolchain_latest_version::StaticInit ();
                get_toolchain_source_toolchain_sha2_256::StaticInit ();
                get_toolchain_source_toolchain_versions::StaticInit ();
                get_toolchain_source_url::StaticInit ();
                update_toolchain_sources::StaticInit ();
            #endif // defined (THEKOGANS_MAKE_HAVE_CURL)
            #if defined (TOOLCHAIN_OS_Windows)
                list_cygwin_mount_table::StaticInit ();
                update_cl_dependencies::StaticInit ();
                update_windows_dependencies::StaticInit ();
            #endif // defined (TOOLCHAIN_OS_Windows)
            }
        #endif // defined (THEKOGANS_MAKE_TYPE_Static)

        } // namespace actions
    } // namespace make
} // namespace thekogans
