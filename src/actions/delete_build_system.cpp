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
            struct delete_build_system : public Action {
                THEKOGANS_MAKE_CORE_DECLARE_ACTION (delete_build_system)

                virtual const char *GetGroup () const {
                    return GROUP_BUILD;
                }

                virtual void PrintHelp (std::ostream &stream) const {
                    stream <<
                        "-a:" << GetName () << " -g:[" << core::GetGeneratorList (" | ") << "] "
                        "-c:[" CONFIG_DEBUG " | " CONFIG_RELEASE "] -t:[" TYPE_STATIC " | " TYPE_SHARED "] "
                        "-r:[" TYPE_STATIC " | " TYPE_SHARED "] [-d] path\n\n"
                        "a - Delete a build system.\n"
                        "g - Build system generator.\n"
                        "c - Build configuration [" CONFIG_DEBUG " | " CONFIG_RELEASE "].\n"
                        "t - Program = Link type, Library = Library type [" TYPE_STATIC " | " TYPE_SHARED "].\n"
                        "r - Runtime library link type [" TYPE_STATIC " | " TYPE_SHARED "].\n"
                        "d - Follow dependencies.\n"
                        "path - Path to " THEKOGANS_MAKE_XML " file.\n";
                }

                virtual void Execute  () {
                    core::DeleteBuildSystem (
                        Options::Instance ().path,
                        Options::Instance ().generator,
                        Options::Instance ().config,
                        Options::Instance ().type,
                        Options::Instance ().runtime_type,
                        Options::Instance ().dependencies);
                }
            };

            THEKOGANS_MAKE_CORE_IMPLEMENT_ACTION (delete_build_system)
        }

    } // namespace make
} // namespace thekogans
