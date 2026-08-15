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

#include "thekogans/make/Version.h"

namespace thekogans {
    namespace make {

        const util::Version &GetVersion () {
            static const util::Version version (
                THEKOGANS_MAKE_MAJOR_VERSION,
                THEKOGANS_MAKE_MINOR_VERSION,
                THEKOGANS_MAKE_PATCH_VERSION);
            return version;
        }

    } // namespace make
} // namespace thekogans
