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

#include "thekogans/make/core/Toolchain.h"
#include "thekogans/make/functions/is_installed.h"

namespace thekogans {
    namespace make {

        THEKOGANS_UTIL_IMPLEMENT_DYNAMIC_CREATABLE (is_installed, Function::TYPE)

        core::Value is_installed::Exec (
                const core::thekogans_make & /*thekogans_make*/,
                const Parameters &parameters) const {
            std::string organization;
            std::string project;
            std::string version;
            for (Parameters::const_iterator
                    it = parameters.begin (),
                    end = parameters.end (); it != end; ++it) {
                if ((*it).first == "o" || (*it).first == "organization") {
                    organization = (*it).second;
                }
                else if ((*it).first == "p" || (*it).first == "project") {
                    project = (*it).second;
                }
                else if ((*it).first == "v" || (*it).first == "version") {
                    version = (*it).second;
                }
            }
            if (version.empty ()) {
                version = core::Toolchain::GetLatestVersion (organization, project);
            }
            return core::Value (core::Toolchain::IsInstalled (organization, project, version));
        }

    } // namespace make
} // namespace thekogans
