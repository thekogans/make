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
#include "thekogans/make/core/Value.h"
#include "thekogans/make/functions/dependency_have_feature.h"

namespace thekogans {
    namespace make {
        namespace functions {

            THEKOGANS_UTIL_IMPLEMENT_DYNAMIC_CREATABLE (dependency_have_feature, Function::TYPE)

            core::Value dependency_have_feature::Exec (
                    const core::thekogans_make &thekogans_make,
                    const Parameters &parameters) const {
                std::string organization;
                std::string name;
                std::string feature;
                for (Parameters::const_iterator
                        it = parameters.begin (),
                        end = parameters.end (); it != end; ++it) {
                    if ((*it).first == "o" || (*it).first == "organization") {
                        organization = (*it).second;
                    }
                    else if ((*it).first == "n" || (*it).first == "name") {
                        name = (*it).second;
                    }
                    else if ((*it).first == "f" || (*it).first == "feature") {
                        feature = (*it).second;
                    }
                }
                const core::thekogans_make::Dependency *dependency =
                    thekogans_make.GetDependency (organization, name);
                return core::Value (dependency != nullptr && dependency->HaveFeature (feature));
            }

        } // namespace functions
    } // namespace make
} // namespace thekogans
