// Copyright 2011 Boris Kogan (boris@thekogans.net)
//
// This file is part of libthekogans_stream.
//
// libthekogans_stream is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libthekogans_stream is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libthekogans_stream. If not, see <http://www.gnu.org/licenses/>.

#if !defined (__thekogans_make_Options_h)
#define __thekogans_make_Options_h

#include <string>
#include "thekogans/util/CommandLineOptions.h"
#include "thekogans/util/Singleton.h"
#include "thekogans/util/SpinLock.h"
#include "thekogans/make/core/Utils.h"

namespace thekogans {
    namespace make {

        struct Options :
                public util::CommandLineOptions,
                public util::Singleton<Options, util::SpinLock> {
            bool help;
            bool showVersion;
            std::string action;
            std::string generator;
            std::string organization;
            std::string project;
            std::string branch;
            std::string version;
            std::string example;
            std::string file;
            std::string SHA2_256;
            std::string config;
            std::string type;
            std::string dependent;
            std::string dependency;
            std::string url;
            bool dependencies;
            std::string mode;
            bool hide_commands;
            bool parallel_build;
            std::string target;
            std::string path;

            Options () :
                help (false),
                showVersion (false),
                dependencies (false),
                mode (MODE_DEVELOPMENT),
                hide_commands (true),
                parallel_build (true),
                target (TARGET_ALL) {}

            virtual void DoOption (
                char option,
                const std::string &value);

            virtual void DoPath (const std::string &path_) {
                path = path_;
            }
        };

    } // namespace make
} // namespace thekogans

#endif // !defined (__thekogans_make_Options_h)
