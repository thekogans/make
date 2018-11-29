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
            struct build_project : public Action {
                THEKOGANS_MAKE_CORE_DECLARE_ACTION (build_project)

                virtual const char *GetGroup () const {
                    return GROUP_BUILD;
                }

                virtual void PrintHelp (std::ostream &stream) const {
                    stream <<
                        "-a:" << GetName () << " -c:[" CONFIG_DEBUG " | " CONFIG_RELEASE "] "
                        "-t:[" TYPE_STATIC " | " TYPE_SHARED "] -r:[" TYPE_STATIC " | " TYPE_SHARED "] "
                        "[-m:[" MODE_DEVELOPMENT " | " MODE_INSTALL "]] [-w:[yes | no]] [-x:[yes | no]] "
                        "[-z:[" TARGET_ALL " | " TARGET_TESTS " | " TARGET_TESTS_SELF " | " TARGET_CLEAN " | " TARGET_CLEAN_SELF " | " TARGET_TAGS " | " TARGET_TAGS_SELF "]] path\n\n"
                        "a - Build a given project.\n"
                        "c - Build configuration [" CONFIG_DEBUG " | " CONFIG_RELEASE "].\n"
                        "t - Program = Link type, Library = Library type [" TYPE_STATIC " | " TYPE_SHARED "].\n"
                        "r - Runtime library link type [" TYPE_STATIC " | " TYPE_SHARED "].\n"
                        "m - Build mode [" MODE_DEVELOPMENT " | " MODE_INSTALL "].\n"
                        "w - Hide commands [yes | no].\n"
                        "x - Parallel build [yes | no].\n"
                        "z - Build target.\n"
                        "path - Path to " THEKOGANS_MAKE_XML " file.\n";
                }

                virtual void Execute  () {
                    core::BuildProject (
                        Options::Instance ().path,
                        Options::Instance ().config,
                        Options::Instance ().type,
                        Options::Instance ().runtime_type,
                        Options::Instance ().mode,
                        Options::Instance ().hide_commands,
                        Options::Instance ().parallel_build,
                        Options::Instance ().target);
                }
            };

            THEKOGANS_MAKE_CORE_IMPLEMENT_ACTION (build_project)
        }

    } // namespace make
} // namespace thekogans
