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
            struct add_source_project : public Action {
                THEKOGANS_MAKE_CORE_DECLARE_ACTION (add_source_project)

                virtual const char *GetGroup () const {
                    return GROUP_SOURCES;
                }

                virtual void PrintHelp (std::ostream &stream) const {
                    stream <<
                        "-a:" << GetName () << " -o:organization -p:project [-b:branch] -v:version -s:SHA2_256\n\n"
                        "a - Add a project entry in $DEVELOPMENT_ROOT/sources/$organization/Source.xml.\n"
                        "o - Organization name.\n"
                        "p - Project name.\n"
                        "b - Project branch.\n"
                        "v - Project version.\n"
                        "s - Project SHA2-256 hash.\n";
                }

                virtual void Execute  () {
                    core::Source source (Options::Instance ().organization);
                    source.AddProject (
                        Options::Instance ().project,
                        Options::Instance ().branch,
                        Options::Instance ().version,
                        Options::Instance ().SHA2_256);
                    source.Save ();
                }
            };

            THEKOGANS_MAKE_CORE_IMPLEMENT_ACTION (add_source_project)
        }

    } // namespace make
} // namespace thekogans
