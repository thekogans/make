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

#include <iostream>
#include "thekogans/make/core/thekogans_make.h"
#include "thekogans/make/Options.h"
#include "thekogans/make/actions/get_build_config.h"

namespace thekogans {
    namespace make {
        namespace actions {

            THEKOGANS_UTIL_IMPLEMENT_DYNAMIC_CREATABLE (get_build_config, Action::TYPE)

            void get_build_config::PrintHelp (std::ostream &stream) const {
                stream <<
                    "-a:" << Type () << " path\n\n"
                    "a - Get the build config (" CONFIG_DEBUG " | " CONFIG_RELEASE ") from a project or toolchain config file.\n"
                    "path - Path to " THEKOGANS_MAKE_XML " or toolchain configfile.\n";
            }

            void get_build_config::Execute () {
                std::cout << core::thekogans_make::GetBuildConfig (
                    std::string (),
                    Options::Instance ()->path);
                std::cout.flush ();
            }

        } // namespace actions
    } // namespace make
} // namespace thekogans
