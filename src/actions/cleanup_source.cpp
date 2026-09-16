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

#include <set>
#include "thekogans/make/core/Source.h"
#include "thekogans/make/Options.h"
#include "thekogans/make/actions/cleanup_source.h"

namespace thekogans {
    namespace make {
        namespace actions {

            THEKOGANS_UTIL_IMPLEMENT_DYNAMIC_CREATABLE (cleanup_source, Action::TYPE)

            void cleanup_source::PrintHelp (std::ostream &stream) const {
                stream <<
                    "-a:" << Type () << " [-o:organization]\n\n"
                    "a - Remove old project and toolchain versions form a given "
                    "$SOURCES_ROOT/$organization/Source.xml.\n"
                    "o - Optional organization name.\n";
            }

            void cleanup_source::Execute () {
                std::set<std::string> sources;
                if (!Options::Instance ()->organization.empty ()) {
                    sources.insert (Options::Instance ()->organization);
                }
                else {
                    core::Source::GetSources (sources);
                }
                for (const auto &source : sources) {
                    core::Source source_ (source);
                    {
                        std::set<std::string> projects;
                        source_.GetProjectNames (projects);
                        for (const auto &project : projects) {
                            std::set<std::string> branches;
                            source_.GetProjectBranches (project, branches);
                            for (const auto &branch : branches) {
                                source_.CleanupProject (project, branch);
                            }
                        }
                    }
                    {
                        std::set<std::string> toolchains;
                        source_.GetToolchainNames (toolchains);
                        for (const auto &toolchain : toolchains) {
                            source_.CleanupToolchain (toolchain);
                        }
                    }
                    source_.Save ();
                }
            }

        } // namespace actions
    } // namespace make
} // namespace thekogans
