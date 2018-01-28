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
#include "thekogans/make/core/thekogans_make.h"
#include "thekogans/make/core/Utils.h"
#include "thekogans/make/functions/have_feature.h"

namespace thekogans {
    namespace make {

        THEKOGANS_MAKE_CORE_IMPLEMENT_FUNCTION (have_feature)

        core::Value have_feature::Exec (
                const core::thekogans_make &thekogans_make,
                const Parameters &parameters) const {
            for (Parameters::const_iterator
                    it = parameters.begin (),
                    end = parameters.end (); it != end; ++it) {
                if ((*it).first == "f" || (*it).first == "feature") {
                    return core::Value (
                        thekogans_make.features.find ((*it).second) != thekogans_make.features.end ());
                }
            }
            THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                "have_feature: missing parameter [%s]",
                "-f | --feature");
        }

    } // namespace make
} // namespace thekogans
