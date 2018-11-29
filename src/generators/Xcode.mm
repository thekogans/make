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

#include <cassert>
#include "thekogans/make/generators/Xcode.h"

namespace thekogans {
    namespace make {

        THEKOGANS_MAKE_CORE_IMPLEMENT_GENERATOR (Xcode)

        bool Xcode::Generate (
                const std::string &project_root,
                const std::string &config,
                const std::string &type,
                const std::string &runtime_type,
                bool generateDependencies,
                bool force) {
            // FIXME: implement
            assert (0);
            return false;
        }

        void Xcode::Delete (
                const std::string &project_root,
                const std::string &config,
                const std::string &type,
                const std::string &runtime_type,
                bool deleteDependencies) {
            // FIXME: implement
            assert (0);
        }

    } // namespace make
} // namespace thekogans
