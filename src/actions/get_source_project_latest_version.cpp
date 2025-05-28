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
            struct get_source_project_latest_version : public Action {
                THEKOGANS_UTIL_DECLARE_DYNAMIC_CREATABLE (get_source_project_latest_version)

                virtual std::string GetGroup () const {
                    return GROUP_SOURCES;
                }

                virtual void PrintHelp (std::ostream &stream) const {
                    stream <<
                        "-a:" << Type () << " -o:organization -p:project [-b:branch]\n\n"
                        "a - Return the latest version of a specified project.\n"
                        "o - Organization name.\n"
                        "p - Project name.\n"
                        "b - Project branch.\n";
                }

                virtual void Execute  () {
                    if (Options::Instance ()->branch.empty ()) {
                        Options::Instance ()->branch = core::GetDefaultBranch (
                            Options::Instance ()->organization,
                            Options::Instance ()->project);
                    }
                    core::Source source (Options::Instance ()->organization);
                    std::cout << source.GetProjectLatestVersion (
                        Options::Instance ()->project,
                        Options::Instance ()->branch);
                    std::cout.flush ();
                }
            };

            THEKOGANS_UTIL_IMPLEMENT_DYNAMIC_CREATABLE (get_source_project_latest_version, Action::TYPE)
        }

    } // namespace make
} // namespace thekogans
