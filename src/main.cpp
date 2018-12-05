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

#include <string>
#include <list>
#include <iostream>
#if defined (THEKOGANS_MAKE_HAVE_CURL)
    #include <curl/curl.h>
#endif // defined (THEKOGANS_MAKE_HAVE_CURL)
#include "thekogans/util/LoggerMgr.h"
#include "thekogans/util/ConsoleLogger.h"
#include "thekogans/util/Exception.h"
#include "thekogans/util/Path.h"
#include "thekogans/util/Plugins.h"
#include "thekogans/make/core/Version.h"
#include "thekogans/make/Options.h"
#include "thekogans/make/Action.h"
#include "thekogans/make/Version.h"

using namespace thekogans;

int main (
        int argc,
        const char *argv[]) {
#if defined (THEKOGANS_MAKE_HAVE_CURL)
    curl_global_init (CURL_GLOBAL_ALL);
#endif // defined (THEKOGANS_MAKE_HAVE_CURL)
    THEKOGANS_UTIL_LOG_INIT (
        util::LoggerMgr::Info,
        util::LoggerMgr::NoDecorations);
    THEKOGANS_UTIL_LOG_ADD_LOGGER (util::Logger::Ptr (new util::ConsoleLogger ()));
    THEKOGANS_UTIL_IMPLEMENT_LOG_FLUSHER;
    make::Options::Instance ().Parse (argc, argv, "hvagopbefsctdyumwxz");
    int result = 0;
    if (make::Options::Instance ().help) {
        if (!make::Options::Instance ().action.empty ()) {
            make::Action::UniquePtr action = make::Action::Get (
                make::Options::Instance ().action);
            if (action.get () != 0) {
                std::cout << argv[0] << " " << *action << std::endl;
            }
        }
        else {
            std::list<std::string> actions;
            make::Action::GetActions (actions);
            for (std::list<std::string>::const_iterator
                    it = actions.begin (),
                    end = actions.end (); it != end; ++it) {
                make::Action::UniquePtr action = make::Action::Get (*it);
                if (action.get () != 0) {
                    std::cout << argv[0] << " " << *action << std::endl;
                }
            }
        }
    }
    else if (make::Options::Instance ().showVersion) {
        std::cout <<
            "libthekogans_util - " << util::GetVersion ().ToString () << "\n" <<
            "libthekogans_make_core - " << make::core::GetVersion ().ToString () << "\n" <<
            argv[0] << " - " << make::GetVersion ().ToString () << "\n";
        std::cout.flush ();
    }
    else if (make::Options::Instance ().action.empty ()) {
        std::cout << "No action specified.\n";
        std::cout.flush ();
        result = 1;
    }
    else {
        THEKOGANS_UTIL_TRY {
            util::Path pluginsPath (ToSystemPath (std::string (argv[0]) + "." + make::core::PLUGINS_EXT));
            if (pluginsPath.Exists ()) {
                util::Plugins plugins (pluginsPath.path);
                plugins.Load ();
            }
            make::Action::UniquePtr action = make::Action::Get (make::Options::Instance ().action);
            if (action.get () != 0) {
                action->Execute ();
            }
            else {
                std::list<std::string> actions;
                make::Action::GetActions (actions);
                std::cout << util::FormatString (
                    "Invlalid action: %s (%s)\n",
                    make::Options::Instance ().action.c_str (),
                    util::FormatList (actions, " | ").c_str ());
                result = 1;
            }
        }
        THEKOGANS_UTIL_CATCH (util::Exception) {
            THEKOGANS_UTIL_LOG_ERROR ("%s\n", exception.Report ().c_str ());
            result = 1;
        }
    }
    return result;
}
