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

#include "thekogans/util/Version.h"
#include "thekogans/make/functions/get_patch_version.h"

namespace thekogans {
    namespace make {

        THEKOGANS_UTIL_IMPLEMENT_DYNAMIC_CREATABLE (get_patch_version, Function::TYPE)

        core::Value get_patch_version::Exec (
                const core::thekogans_make & /*thekogans_make*/,
                const Parameters &parameters) const {
            std::string version;
            for (Parameters::const_iterator
                    it = parameters.begin (),
                    end = parameters.end (); it != end; ++it) {
                if ((*it).first == "v" || (*it).first == "version") {
                    version = (*it).second;
                }
            }
            return core::Value (util::ui32Tostring (util::Version (version).patchVersion));
        }

    } // namespace make
} // namespace thekogans
