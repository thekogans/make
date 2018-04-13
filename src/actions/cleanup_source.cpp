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
            struct cleanup_source : public Action {
                THEKOGANS_MAKE_CORE_DECLARE_ACTION (cleanup_source)

                virtual const char *GetGroup () const {
                    return GROUP_SOURCES;
                }

                virtual void PrintHelp (std::ostream &stream) const {
                    stream <<
                        "-a:" << GetName () << " [-o:organization]\n\n"
                        "a - Remove old project and toolchain versions form a given "
                        "$DEVELOPMENT_ROOT/sources/$organization/Source.xml.\n"
                        "o - Optional organization name.\n";
                }

                virtual void Execute  () {
                    std::set<std::string> sources;
                    if (!Options::Instance ().organization.empty ()) {
                        sources.insert (Options::Instance ().organization);
                    }
                    else {
                        core::Source::GetSources (sources);
                    }
                    for (std::set<std::string>::const_iterator
                            it = sources.begin (),
                            end = sources.end (); it != end; ++it) {
                        core::Source source (*it);
                        {
                            std::set<std::string> projects;
                            source.GetProjectNames (projects);
                            for (std::set<std::string>::const_iterator
                                    jt = projects.begin (),
                                    end = projects.end (); jt != end; ++jt) {
                                std::set<std::string> branches;
                                source.GetProjectBranches (*jt, branches);
                                for (std::set<std::string>::const_iterator
                                        kt = branches.begin (),
                                        end = branches.end (); kt != end; ++kt) {
                                    source.CleanupProject (*jt, *kt);
                                }
                            }
                        }
                        {
                            std::set<std::string> toolchain;
                            source.GetToolchainNames (toolchain);
                            for (std::set<std::string>::const_iterator
                                    jt = toolchain.begin (),
                                    end = toolchain.end (); jt != end; ++jt) {
                                source.CleanupToolchain (*jt);
                            }
                        }
                        source.Save ();
                    }
                }
            };

            THEKOGANS_MAKE_CORE_IMPLEMENT_ACTION (cleanup_source)
        }

    } // namespace make
} // namespace thekogans
