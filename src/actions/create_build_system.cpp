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
            struct create_build_system : public Action {
                THEKOGANS_MAKE_DECLARE_ACTION (create_build_system)

                virtual const char *GetGroup () const {
                    return GROUP_BUILD;
                }

                virtual void PrintHelp (std::ostream &stream) const {
                    stream <<
                        "-a:" << GetName () << " -g:[" << make::core::GetGeneratorList (" | ") << "] "
                        "-c:[" CONFIG_DEBUG " | " CONFIG_RELEASE "] -t:[" TYPE_STATIC " | " TYPE_SHARED "] [-d] [-f] path\n\n"
                        "a - Create a build system.\n"
                        "g - Build system generator.\n"
                        "c - Build configuration [" CONFIG_DEBUG " | " CONFIG_RELEASE "].\n"
                        "t - Program = Link type, Library = Library type [" TYPE_STATIC " | " TYPE_SHARED "].\n"
                        "d - Follow dependencies.\n"
                        "f - Don't bother checking the timestamps and force generation.\n"
                        "path - Path to " THEKOGANS_MAKE_XML " file.\n";
                }

                virtual void Execute  () {
                    core::CreateBuildSystem (
                        Options::Instance ()->path,
                        Options::Instance ()->generator,
                        Options::Instance ()->config,
                        Options::Instance ()->type,
                        Options::Instance ()->dependencies,
                        Options::Instance ()->force);
                }
            };

            THEKOGANS_MAKE_IMPLEMENT_ACTION (create_build_system)
        }

    } // namespace make
} // namespace thekogans
