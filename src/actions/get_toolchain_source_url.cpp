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
            struct get_toolchain_source_url : public Action {
                THEKOGANS_MAKE_DECLARE_ACTION (get_toolchain_source_url)

                virtual const char *GetGroup () const {
                    return GROUP_TOOLCHAIN_SOURCES_XML;
                }

                virtual void PrintHelp (std::ostream &stream) const {
                    stream <<
                        "-a:" << GetName () << " -o:organization\n\n"
                        "a - Return the URL associated with the given source.\n"
                        "o - Source organization name.\n";
                }

                virtual void Execute  () {
                    std::cout << core::ToolchainSources::Instance ()->GetSourceURL (
                        Options::Instance ()->organization);
                    std::cout.flush ();
                }
            };

            THEKOGANS_MAKE_IMPLEMENT_ACTION (get_toolchain_source_url)
        }

    } // namespace make
} // namespace thekogans
