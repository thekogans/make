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

#include "thekogans/util/Exception.h"
#include "thekogans/make/core/Toolchain.h"
#include "thekogans/make/functions/get_toolchain_library.h"

namespace thekogans {
    namespace make {

        THEKOGANS_UTIL_IMPLEMENT_DYNAMIC_CREATABLE (get_toolchain_library, Function::TYPE)

        core::Value get_toolchain_library::Exec (
                const core::thekogans_make & /*thekogans_make*/,
                const Parameters &parameters) const {
            std::string organization;
            std::string project;
            std::string version;
            std::string config;
            std::string type;
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
                else if ((*it).first == "c" || (*it).first == "config") {
                    config = (*it).second;
                }
                else if ((*it).first == "t" || (*it).first == "type") {
                    type = (*it).second;
                }
            }
            if (core::Toolchain::Find (organization, project, version)) {
                return core::Value (
                    core::Toolchain::GetLibrary (organization, project, version, config, type));
            }
            else {
                THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                    "Unable to find: %s",
                    core::Toolchain::GetConfig (organization, project, version).c_str ());
            }
        }

    } // namespace make
} // namespace thekogans
