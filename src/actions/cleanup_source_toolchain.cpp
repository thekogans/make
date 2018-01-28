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

#include <string>
#include <set>
#include <iostream>
#include "thekogans/make/core/Source.h"
#include "thekogans/make/Options.h"
#include "thekogans/make/Action.h"

namespace thekogans {
    namespace make {

        namespace {
            struct cleanup_source_toolchain : public Action {
                THEKOGANS_MAKE_CORE_DECLARE_ACTION (cleanup_source_toolchain)

                virtual const char *GetGroup () const {
                    return GROUP_SOURCES;
                }

                virtual void PrintHelp (std::ostream &stream) const {
                    stream <<
                        "-a:" << GetName () << " -o:organization -p:project -b:branch\n\n"
                        "a - Remove old versions associated with the given toolchain in $DEVELOPMENT_ROOT/sources/$organization/Source.xml.\n"
                        "o - Organization name.\n"
                        "p - Project name.\n";
                }

                virtual void Execute  () {
                    core::Source source (Options::Instance ().organization);
                    std::set<std::string> toolchain;
                    if (!Options::Instance ().project.empty ()) {
                        toolchain.insert (Options::Instance ().project);
                    }
                    else {
                        source.GetToolchainNames (toolchain);
                    }
                    for (std::set<std::string>::const_iterator
                            it = toolchain.begin (),
                            end = toolchain.end (); it != end; ++it) {
                        source.CleanupToolchain (*it);
                    }
                    source.Save ();
                }
            };

            THEKOGANS_MAKE_CORE_IMPLEMENT_ACTION (cleanup_source_toolchain)
        }

    } // namespace make
} // namespace thekogans
