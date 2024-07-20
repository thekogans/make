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
#include "thekogans/make/core/Installer.h"
#include "thekogans/make/Options.h"
#include "thekogans/make/Action.h"

namespace thekogans {
    namespace make {

        namespace {
            struct install_program : public Action {
                THEKOGANS_MAKE_DECLARE_ACTION (install_program)

                virtual const char *GetGroup () const {
                    return GROUP_INSTALL;
                }

                virtual void PrintHelp (std::ostream &stream) const {
                    stream <<
                        "-a:" << GetName () << " -c:[" CONFIG_DEBUG " | " CONFIG_RELEASE "] "
                        "-t:[" TYPE_STATIC " | " TYPE_SHARED "] [-w:[yes | no]] [-x:[yes | no]] path\n\n"
                        "a - Install the given program in to the toolchain.\n"
                        "c - Build configuration [" CONFIG_DEBUG " | " CONFIG_RELEASE "].\n"
                        "t - Link type [" TYPE_STATIC " | " TYPE_SHARED "].\n"
                        "w - Hide commands [yes | no].\n"
                        "x - Parallel build [yes | no].\n"
                        "path - Path to " THEKOGANS_MAKE_XML " file.\n";
                }

                virtual void Execute  () {
                    core::Installer installer (
                        Options::Instance ()->config,
                        Options::Instance ()->type,
                        Options::Instance ()->hide_commands,
                        Options::Instance ()->parallel_build);
                    installer.InstallProgram (Options::Instance ()->path);
                }
            };

            THEKOGANS_MAKE_IMPLEMENT_ACTION (install_program)
        }

    } // namespace make
} // namespace thekogans
