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
#include "thekogans/make/actions/get_source_toolchain_versions.h"

namespace thekogans {
    namespace make {
        namespace actions {

            THEKOGANS_UTIL_IMPLEMENT_DYNAMIC_CREATABLE (get_source_toolchain_versions, Action::TYPE)

            void get_source_toolchain_versions::PrintHelp (std::ostream &stream) const {
                stream <<
                    "-a:" << Type () << " -o:organization -p:project\n\n"
                    "a - Return all versions of a specified toolchain library/program/plugin "
                    "from source $SOURCES_ROOT/$organization/Source.xml.\n"
                    "o - Organization name.\n"
                    "p - Project name.\n";
            }

            void get_source_toolchain_versions::Execute () {
                core::Source source (Options::Instance ()->organization);
                std::set<std::string> versions;
                source.GetToolchainVersions (
                    Options::Instance ()->project,
                    versions);
                for (std::set<std::string>::const_iterator
                         it = versions.begin (),
                         end = versions.end (); it != end; ++it) {
                    std::cout << *it << std::endl;
                }
                std::cout.flush ();
            }

        } // namespace actions
    } // namespace make
} // namespace thekogans
