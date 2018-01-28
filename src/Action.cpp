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

        namespace {
            // Believe it or not, but just declaring map static
            // does not guarantee proper ctor call order!? Wrapping
            // it in an accessor function does.
            Action::Map &GetMap () {
                static Action::Map map;
                return map;
            }
        }

        Action::UniquePtr Action::Get (const std::string &type) {
            Map::iterator it = GetMap ().find (type);
            return it != GetMap ().end () ?
                it->second () : Action::UniquePtr ();
        }

        Action::MapInitializer::MapInitializer (
                const std::string &type,
                Factory factory) {
            std::pair<Map::iterator, bool> result =
                GetMap ().insert (Map::value_type (type, factory));
            assert (result.second);
            if (!result.second) {
                THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                    "Duplicate Action: %s", type.c_str ());
            }
        }

        void Action::GetActions (std::list<std::string> &actions) {
            for (Map::const_iterator
                    it = GetMap ().begin (),
                    end = GetMap ().end (); it != end; ++it) {
                actions.push_back (it->first);
            }
        }

        void Action::GetGroups (std::list<std::string> &groups) {
            std::set<std::string> groups_;
            for (Map::const_iterator
                    it = GetMap ().begin (),
                    end = GetMap ().end (); it != end; ++it) {
                UniquePtr action = it->second ();
                if (groups_.find (action->GetGroup ()) == groups_.end ()) {
                    groups_.insert (action->GetGroup ());
                    groups.push_back (action->GetGroup ());
                }
            }
        }

        void Action::GetGroupActions (
                const std::string &group,
                std::list<std::string> &actions) {
            std::set<std::string> groups_;
            for (Map::const_iterator
                    it = GetMap ().begin (),
                    end = GetMap ().end (); it != end; ++it) {
                UniquePtr action = it->second ();
                if (action->GetGroup () == group) {
                    actions.push_back (it->first);
                }
            }
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
