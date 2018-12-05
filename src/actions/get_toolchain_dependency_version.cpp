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
#include "thekogans/make/core/thekogans_make.h"
#include "thekogans/make/Options.h"
#include "thekogans/make/Action.h"

namespace thekogans {
    namespace make {

        namespace {
            struct get_toolchain_dependency_version : public Action {
                THEKOGANS_MAKE_DECLARE_ACTION (get_toolchain_dependency_version)

                virtual const char *GetGroup () const {
                    return GROUP_TOOLCHAIN_SOURCES_XML;
                }

                virtual void PrintHelp (std::ostream &stream) const {
                    stream <<
                        "-a:" << GetName () << " -o:organization -p:project "
                        "-c:[" CONFIG_DEBUG " | " CONFIG_RELEASE "] -t:[" TYPE_STATIC " | " TYPE_SHARED "] path\n\n"
                        "a - Get the toolchain dependency version.\n"
                        "o - Dependency organization name.\n"
                        "p - Dependency project name.\n"
                        "c - Project build configuration [" CONFIG_DEBUG " | " CONFIG_RELEASE "].\n"
                        "t - Project link type [" TYPE_STATIC " | " TYPE_SHARED "].\n"
                        "path - Path to " THEKOGANS_MAKE_XML " or toolchain configfile.\n";
                }

                virtual void Execute  () {
                    const core::thekogans_make &thekogans_make =
                        core::thekogans_make::GetConfig (
                            std::string (),
                            Options::Instance ().path,
                            Options::Instance ().generator,
                            Options::Instance ().config,
                            Options::Instance ().type);
                    std::cout << thekogans_make.GetToolchainDependencyVersion (
                        Options::Instance ().organization,
                        Options::Instance ().project);
                    std::cout.flush ();
                }
            };

            THEKOGANS_MAKE_IMPLEMENT_ACTION (get_toolchain_dependency_version)
        }

    } // namespace make
} // namespace thekogans
