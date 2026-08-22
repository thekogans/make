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

#if !defined (__thekogans_make_actions_copy_dependencies_h)
#define __thekogans_make_actions_copy_dependencies_h

#include "thekogans/make/Action.h"

namespace thekogans {
    namespace make {
        namespace actions {

            struct copy_dependencies : public Action {
                THEKOGANS_UTIL_DECLARE_DYNAMIC_CREATABLE (copy_dependencies)

                virtual std::string GetGroup () const override {
                    return GROUP_INSTALL;
                }

                virtual void PrintHelp (std::ostream &stream) const override;
                virtual void Execute () override;
            };

        } // namespace actions
    } // namespace make
} // namespace thekogans

#endif // !defined (__thekogans_make_actions_copy_dependencies_h)
