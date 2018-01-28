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
            struct get_source_project_sha2_256 : public Action {
                THEKOGANS_MAKE_CORE_DECLARE_ACTION (get_source_project_sha2_256)

                virtual const char *GetGroup () const {
                    return GROUP_SOURCES;
                }

                virtual void PrintHelp (std::ostream &stream) const {
                    stream <<
                        "-a:" << GetName () << " -o:organization -p:project [-b:branch] [-v:version]\n\n"
                        "a - Return the SHA2-256 hash of a specified project.\n"
                        "o - Organization name.\n"
                        "p - Project name.\n"
                        "b - Project branch.\n"
                        "v - Project version.\n";
                }

                virtual void Execute  () {
                    if (Options::Instance ().branch.empty ()) {
                        Options::Instance ().branch = core::GetDefaultBranch (
                            Options::Instance ().organization,
                            Options::Instance ().project);
                    }
                    core::Source source (Options::Instance ().organization);
                    if (Options::Instance ().version.empty ()) {
                        Options::Instance ().version =
                            source.GetProjectLatestVersion (
                                Options::Instance ().project,
                                Options::Instance ().branch);
                    }
                    std::cout << source.GetProjectSHA2_256 (
                        Options::Instance ().project,
                        Options::Instance ().branch,
                        Options::Instance ().version);
                    std::cout.flush ();
                }
            };

            THEKOGANS_MAKE_CORE_IMPLEMENT_ACTION (get_source_project_sha2_256)
        }

    } // namespace make
} // namespace thekogans
