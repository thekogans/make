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
#include "thekogans/make/core/Sources.h"
#include "thekogans/make/core/Utils.h"
#include "thekogans/make/Options.h"
#include "thekogans/make/actions/get_toolchain_source_project_latest_version.h"

namespace thekogans {
    namespace make {

        THEKOGANS_UTIL_IMPLEMENT_DYNAMIC_CREATABLE (get_toolchain_source_project_latest_version, Action::TYPE)

        void get_toolchain_source_project_latest_version::PrintHelp (std::ostream &stream) const {
            stream <<
                "-a:" << Type () << " -o:organization -p:project [-b:branch]\n\n"
                "a - Return the latest version of a specified project.\n"
                "o - Organization name.\n"
                "p - Project name.\n"
                "b - Project branch.\n";
        }

        void get_toolchain_source_project_latest_version::Execute () {
            if (Options::Instance ()->branch.empty ()) {
                Options::Instance ()->branch = core::GetDefaultBranch (
                    Options::Instance ()->organization,
                    Options::Instance ()->project);
            }
            std::cout << core::ToolchainSources::Instance ()->GetSourceProjectLatestVersion (
                Options::Instance ()->organization,
                Options::Instance ()->project,
                Options::Instance ()->branch);
            std::cout.flush ();
        }

    } // namespace make
} // namespace thekogans
