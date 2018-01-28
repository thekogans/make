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
#include "thekogans/make/Action.h"

namespace thekogans {
    namespace make {

        namespace {
            struct add_toolchain_source : public Action {
                THEKOGANS_MAKE_CORE_DECLARE_ACTION (add_toolchain_source)

                virtual const char *GetGroup () const {
                    return GROUP_TOOLCHAIN_SOURCES_XML;
                }

                virtual void PrintHelp (std::ostream &stream) const {
                    stream <<
                        "-a:" << GetName () << " -o:organization -u:url\n\n"
                        "a - Add a new organization to the $TOOLCHAIN_ROOT/Sources.xml file.\n"
                        "o - Name of organization to add as source.\n"
                        "u - Organization URL.\n";
                }

                virtual void Execute  () {
                    core::ToolchainSources::Instance ().AddSource (
                        Options::Instance ().organization,
                        Options::Instance ().url);
                }
            };

            THEKOGANS_MAKE_CORE_IMPLEMENT_ACTION (add_toolchain_source)
        }

    } // namespace make
} // namespace thekogans
