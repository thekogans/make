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
#include "thekogans/make/Action.h"

namespace thekogans {
    namespace make {

        namespace {
            struct create_source : public Action {
                THEKOGANS_UTIL_DECLARE_DYNAMIC_CREATABLE (create_source)

                virtual std::string GetGroup () const {
                    return GROUP_SOURCES;
                }

                virtual void PrintHelp (std::ostream &stream) const {
                    stream <<
                        "-a:" << Type () << " -o:organization -u:url\n\n"
                        "a - Create a new source in $DEVELOPMENT_ROOT/sources.\n"
                        "o - Organization name.\n"
                        "u - Organization URL.\n";
                }

                virtual void Execute  () {
                    core::Source::Create (
                        Options::Instance ()->organization,
                        Options::Instance ()->url);
                }
            };

            THEKOGANS_UTIL_IMPLEMENT_DYNAMIC_CREATABLE (create_source, Action::TYPE)
        }

    } // namespace make
} // namespace thekogans
