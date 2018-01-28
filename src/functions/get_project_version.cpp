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

#include "thekogans/make/core/thekogans_make.h"
#include "thekogans/make/core/Utils.h"
#include "thekogans/make/functions/get_project_root.h"
#include "thekogans/make/functions/get_project_version.h"

namespace thekogans {
    namespace make {

        THEKOGANS_MAKE_CORE_IMPLEMENT_FUNCTION (get_project_version)

        core::Value get_project_version::Exec (
                const core::thekogans_make &thekogans_make,
                const Parameters &parameters) const {
            return
                core::Value (
                    core::Value::TYPE_Version,
                    core::thekogans_make::GetVersion (
                        get_project_root ().Exec (thekogans_make, parameters).ToString (),
                        THEKOGANS_MAKE_XML));
        }

    } // namespace make
} // namespace thekogans
