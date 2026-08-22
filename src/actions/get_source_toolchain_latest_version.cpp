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
#include "thekogans/make/core/Source.h"
#include "thekogans/make/Options.h"
#include "thekogans/make/actions/get_source_toolchain_latest_version.h"

namespace thekogans {
    namespace make {
        namespace actions {

            THEKOGANS_UTIL_IMPLEMENT_DYNAMIC_CREATABLE (get_source_toolchain_latest_version, Action::TYPE)

            void get_source_toolchain_latest_version::PrintHelp (std::ostream &stream) const {
                stream <<
                    "-a:" << Type () << " -o:organization -p:project\n\n"
                    "a - Return the latest version of a specified toolchain library/program/plugin.\n"
                    "o - Organization name.\n"
                    "p - Project name.\n";
            }

            void get_source_toolchain_latest_version::Execute () {
                core::Source source (Options::Instance ()->organization);
                std::cout <<
                    source.GetToolchainLatestVersion (Options::Instance ()->project);
                std::cout.flush ();
            }

        } // namespace actions
    } // namespace make
} // namespace thekogans
