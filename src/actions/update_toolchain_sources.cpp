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

#if defined (THEKOGANS_MAKE_HAVE_CURL)

#include <iostream>
#include "thekogans/make/core/Sources.h"
#include "thekogans/make/Options.h"
#include "thekogans/make/actions/update_toolchain_sources.h"

namespace thekogans {
    namespace make {
        namespace actions {

            THEKOGANS_UTIL_IMPLEMENT_DYNAMIC_CREATABLE (update_toolchain_sources, Action::TYPE)

            void update_toolchain_sources::PrintHelp (std::ostream &stream) const {
                stream <<
                    "-a:" << Type () << " [-o:organization]\n\n"
                    "a - Update the $TOOLCHAIN_ROOT/Sources.xml file.\n"
                    "o - Organization name.\n";
            }

            void update_toolchain_sources::Execute () {
                core::ToolchainSources::Instance ()->UpdateSources (
                    Options::Instance ()->organization);
            }

        } // namespace actions
    } // namespace make
} // namespace thekogans

#endif // defined (THEKOGANS_MAKE_HAVE_CURL)
