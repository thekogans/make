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
#include "thekogans/make/core/Sources.h"
#include "thekogans/make/Options.h"
#include "thekogans/make/actions/delete_toolchain_source.h"

namespace thekogans {
    namespace make {
        namespace actions {

            THEKOGANS_UTIL_IMPLEMENT_DYNAMIC_CREATABLE (delete_toolchain_source, Action::TYPE)

            void delete_toolchain_source::PrintHelp (std::ostream &stream) const {
                stream <<
                    "-a:" << Type () << " -o:organization\n\n"
                    "a - Delete the given organization from the $TOOLCHAIN_ROOT/Sources.xml file.\n"
                    "o - Name of organization to delete as source.\n";
            }

            void delete_toolchain_source::Execute () {
                core::ToolchainSources::Instance ()->DeleteSource (
                    Options::Instance ()->organization);
            }

        } // namespace actions
    } // namespace make
} // namespace thekogans
