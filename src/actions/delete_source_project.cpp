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
            struct delete_source_project : public Action {
                THEKOGANS_UTIL_DECLARE_DYNAMIC_CREATABLE (delete_source_project)

                virtual std::string GetGroup () const {
                    return GROUP_SOURCES;
                }

                virtual void PrintHelp (std::ostream &stream) const {
                    stream <<
                        "-a:" << Type () << " -o:organization -p:project [-b:branch] [-v:version]\n\n"
                        "a - Delete a project entry in $DEVELOPMENT_ROOT/sources/$organization/Source.xml.\n"
                        "o - Organization name.\n"
                        "p - Project name.\n"
                        "b - Project branch.\n"
                        "v - Project version. Empty = Delete all versions.\n";
                }

                virtual void Execute  () {
                    core::Source source (Options::Instance ()->organization);
                    std::set<std::string> branches;
                    if (!Options::Instance ()->branch.empty ()) {
                        branches.insert (Options::Instance ()->branch);
                    }
                    else {
                        source.GetProjectBranches (Options::Instance ()->project, branches);
                    }
                    for (std::set<std::string>::const_iterator
                            it = branches.begin (),
                            end = branches.end (); it != end; ++it) {
                        std::set<std::string> versions;
                        if (!Options::Instance ()->version.empty ()) {
                            versions.insert (Options::Instance ()->version);
                        }
                        else {
                            source.GetProjectVersions (Options::Instance ()->project, *it, versions);
                        }
                        for (std::set<std::string>::const_iterator
                                jt = versions.begin (),
                                end = versions.end (); jt != end; ++jt) {
                            source.DeleteProject (Options::Instance ()->project, *it, *jt);
                        }
                    }
                    source.Save ();
                }
            };

            THEKOGANS_UTIL_IMPLEMENT_DYNAMIC_CREATABLE (delete_source_project, Action::TYPE)
        }

    } // namespace make
} // namespace thekogans
