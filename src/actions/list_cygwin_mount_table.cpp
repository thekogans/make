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
#include "thekogans/util/StringUtils.h"
#include "thekogans/make/core/CygwinMountTable.h"
#include "thekogans/make/Action.h"

namespace thekogans {
    namespace make {

        namespace {
            struct list_cygwin_mount_table : public Action {
                THEKOGANS_UTIL_DECLARE_DYNAMIC_CREATABLE (list_cygwin_mount_table)

                virtual std::string GetGroup () const {
                    return GROUP_WINDOWS;
                }

                virtual void PrintHelp (std::ostream &stream) const {
                    stream <<
                        "-a:" << Type () << "\n\n"
                        "a - Dump " << util::GetEnvironmentVariable ("TOOLCHAIN_CYGWIN_MOUNT_TABLE") << " maps.\n";
                }

                virtual void Execute  () {
                    core::CygwinMountTable::Instance ()->DumpEntries ();
                }
            };

            THEKOGANS_UTIL_IMPLEMENT_DYNAMIC_CREATABLE (list_cygwin_mount_table, Action::TYPE)
        }

    } // namespace make
} // namespace thekogans
