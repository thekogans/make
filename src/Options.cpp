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

#include "thekogans/make/Options.h"

namespace thekogans {
    namespace make {

        void Options::DoOption (
                char option,
                const std::string &value) {
            switch (option) {
                case 'h':
                    help = true;
                    action = value;
                    break;
                case 'a':
                    action = value;
                    break;
                case 'g':
                    generator = value;
                    break;
                case 'o':
                    organization = value;
                    break;
                case 'p':
                    project = value;
                    break;
                case 'b':
                    branch = value;
                    break;
                case 'v':
                    if (value.empty ()) {
                        showVersion = true;
                    }
                    else {
                        version = value;
                    }
                    break;
                case 'e':
                    example = value;
                    break;
                case 'f':
                    if (value.empty ()) {
                        force = true;
                    }
                    else {
                        file = value;
                    }
                    break;
                case 's':
                    SHA2_256 = value;
                    break;
                case 'c':
                    config = value;
                    break;
                case 't':
                    type = value;
                    break;
                case 'r':
                    runtime_type = value;
                    break;
                case 'd':
                    if (!value.empty ()) {
                        dependent = value;
                    }
                    else {
                        dependencies = true;
                    }
                    break;
                case 'y':
                    dependency = value;
                    break;
                case 'u':
                    url = value;
                    break;
                case 'm':
                    mode = value;
                    break;
                case 'w':
                    hide_commands = value == VALUE_YES;
                    break;
                case 'x':
                    parallel_build = value == VALUE_YES;
                    break;
                case 'z':
                    target = value;
                    break;
            }
        }

    } // namespace make
} // namespace thekogans
