// Copyright 2011 Boris Kogan (boris@thekogans.net)
//
// This file is part of thekogans_make_core.
//
// thekogans_make_core is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// thekogans_make_core is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with thekogans_make_core. If not, see <http://www.gnu.org/licenses/>.

#include <cassert>
#include <set>
#include "thekogans/util/Exception.h"
#include "thekogans/util/LoggerMgr.h"
#include "thekogans/make/Action.h"

namespace thekogans {
    namespace make {

        THEKOGANS_UTIL_IMPLEMENT_DYNAMIC_CREATABLE_ABSTRACT_BASE (thekogans::make::Action)

        std::list<std::string> Action::GetGroups () {
            std::list<std::string> groupList;
            std::set<std::string> groupSet;
            for (TypeMapType::const_iterator
                    it = GetTypes ().begin (),
                    end = GetTypes ().end (); it != end; ++it) {
                SharedPtr action = it->second (nullptr);
                if (action != nullptr &&
                        groupSet.find (action->GetGroup ()) == groupSet.end ()) {
                    groupSet.insert (action->GetGroup ());
                    groupList.push_back (action->GetGroup ());
                }
            }
            return groupList;
        }

        util::DynamicCreatable::TypeMapType Action::GetGroupActions (
                const std::string &group) {
            TypeMapType actions;
            std::set<std::string> groups;
            for (TypeMapType::const_iterator
                    it = GetTypes ().begin (),
                    end = GetTypes ().end (); it != end; ++it) {
                SharedPtr action = it->second (nullptr);
                if (action != nullptr && action->GetGroup () == group) {
                    actions[it->first] = it->second;
                }
            }
            return actions;
        }

        const char * const Action::GROUP_BUILD = "build";
        const char * const Action::GROUP_WINDOWS = "windows";
        const char * const Action::GROUP_INSTALL = "install";
        const char * const Action::GROUP_TOOLCHAIN_SOURCES_XML = "toolchain Sources.xml";
        const char * const Action::GROUP_SOURCES = "sources";
        const char * const Action::GROUP_THEKOGANS_MAKE_XML = "thekogans_make.xml";
        const char * const Action::GROUP_TOOLCHAIN = "toolchain";

    } // namespace make
} // namespace thekogans
