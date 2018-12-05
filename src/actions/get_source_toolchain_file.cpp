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
            struct get_source_toolchain_file : public Action {
                THEKOGANS_MAKE_DECLARE_ACTION (get_source_toolchain_file)

                virtual const char *GetGroup () const {
                    return GROUP_SOURCES;
                }

                virtual void PrintHelp (std::ostream &stream) const {
                    stream <<
                        "-a:" << GetName () << " -o:organization -p:project -v:version\n\n"
                        "a - Return the file of a specified toolchain library/program/plugin.\n"
                        "o - Organization name.\n"
                        "p - Project name.\n"
                        "v - Project version.\n";
                }

                virtual void Execute  () {
                    core::Source source (Options::Instance ().organization);
                    if (Options::Instance ().version.empty ()) {
                        Options::Instance ().version =
                            source.GetToolchainLatestVersion (Options::Instance ().project);
                    }
                    std::cout << source.GetToolchainFile (
                        Options::Instance ().project,
                        Options::Instance ().version);
                    std::cout.flush ();
                }
            };

            THEKOGANS_MAKE_IMPLEMENT_ACTION (get_source_toolchain_file)
        }

    } // namespace make
} // namespace thekogans
