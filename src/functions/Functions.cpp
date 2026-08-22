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
    #include "thekogans/make/functions/get_directory_name.h"
    #include "thekogans/make/functions/get_directory.h"
    #include "thekogans/make/functions/get_extension.h"
    #include "thekogans/make/functions/get_file_name.h"
    #include "thekogans/make/functions/get_full_file_name.h"
    #include "thekogans/make/functions/get_major_version.h"
    #include "thekogans/make/functions/get_minor_version.h"
    #include "thekogans/make/functions/get_patch_version.h"
    #include "thekogans/make/functions/get_project_library.h"
    #include "thekogans/make/functions/get_project_program.h"
    #include "thekogans/make/functions/get_project_root.h"
    #include "thekogans/make/functions/get_project_version.h"
    #include "thekogans/make/functions/get_toolchain_include_dir.h"
    #include "thekogans/make/functions/get_toolchain_latest_version.h"
    #include "thekogans/make/functions/get_toolchain_lib_dir.h"
    #include "thekogans/make/functions/get_toolchain_library.h"
    #include "thekogans/make/functions/get_toolchain_program.h"
    #include "thekogans/make/functions/have_feature.h"
    #include "thekogans/make/functions/is_defined.h"
    #include "thekogans/make/functions/is_installed.h"
    #include "thekogans/make/functions/make_path.h"
    #include "thekogans/make/functions/path_exists.h"
    #include "thekogans/make/functions/to_project_build_path.h"
    #include "thekogans/make/functions/to_project_path.h"
    #include "thekogans/make/functions/to_system_path.h"
#endif // defined (THEKOGANS_MAKE_TYPE_Static)

namespace thekogans {
    namespace make {

    #if defined (THEKOGANS_MAKE_TYPE_Static)
        static void Actions::StaticInit () {
            get_directory_name::StaticInit ();
            get_directory::StaticInit ();
            get_extension::StaticInit ();
            get_file_name::StaticInit ();
            get_full_file_name::StaticInit ();
            get_major_version::StaticInit ();
            get_minor_version::StaticInit ();
            get_patch_version::StaticInit ();
            get_project_library::StaticInit ();
            get_project_program::StaticInit ();
            get_project_root::StaticInit ();
            get_project_version::StaticInit ();
            get_toolchain_include_dir::StaticInit ();
            get_toolchain_latest_version::StaticInit ();
            get_toolchain_lib_dir::StaticInit ();
            get_toolchain_library::StaticInit ();
            get_toolchain_program::StaticInit ();
            have_feature::StaticInit ();
            is_defined::StaticInit ();
            is_installed::StaticInit ();
            make_path::StaticInit ();
            path_exists::StaticInit ();
            to_project_build_path::StaticInit ();
            to_project_path::StaticInit ();
            to_system_path::StaticInit ();
        }
    #endif // defined (THEKOGANS_MAKE_TYPE_Static)

    } // namespace make
} // namespace thekogans
