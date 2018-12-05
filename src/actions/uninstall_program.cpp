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
#include "thekogans/make/core/Utils.h"
#include "thekogans/make/Options.h"
#include "thekogans/make/Action.h"

namespace thekogans {
    namespace make {

        namespace {
            struct uninstall_program : public Action {
                THEKOGANS_MAKE_DECLARE_ACTION (uninstall_program)

                virtual const char *GetGroup () const {
                    return GROUP_INSTALL;
                }

                virtual void PrintHelp (std::ostream &stream) const {
                    stream <<
                        "-a:" << GetName () << " -o:organization -p:project [-v:version] [-d]\n\n"
                        "a - Uninstall a toolchain program.\n"
                        "o - Organization name.\n"
                        "p - Project name.\n"
                        "v - Project version.\n"
                        "d - Delete dependencies.\n";
                }

                virtual void Execute  () {
                    core::UninstallProgram (
                        Options::Instance ().organization,
                        Options::Instance ().project,
                        Options::Instance ().version,
                        Options::Instance ().dependencies);
                }
            };

            THEKOGANS_MAKE_IMPLEMENT_ACTION (uninstall_program)
        }

    } // namespace make
} // namespace thekogans
