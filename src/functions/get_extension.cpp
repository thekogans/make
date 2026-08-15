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

#include "thekogans/util/Path.h"
#include "thekogans/util/Exception.h"
#include "thekogans/make/core/Utils.h"
#include "thekogans/make/functions/get_extension.h"

namespace thekogans {
    namespace make {

        THEKOGANS_UTIL_IMPLEMENT_DYNAMIC_CREATABLE (get_extension, Function::TYPE)

        core::Value get_extension::Exec (
                const core::thekogans_make & /*thekogans_make*/,
                const Parameters &parameters) const {
            for (Parameters::const_iterator
                    it = parameters.begin (),
                    end = parameters.end (); it != end; ++it) {
                if ((*it).first == "p" || (*it).first == "path") {
                    return core::Value (util::Path ((*it).second).GetExtension ());
                }
            }
            THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                "get_extension: missing parameter [%s]",
                "-p | --path");
        }

    } // namespace make
} // namespace thekogans
