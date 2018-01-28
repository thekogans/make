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

#include <iostream>
#include <fstream>
#include "thekogans/make/core/CygwinMountTable.h"
#include "thekogans/make/Options.h"
#include "thekogans/make/Action.h"

namespace thekogans {
    namespace make {

        namespace {
            struct update_windows_dependencies : public Action {
                THEKOGANS_MAKE_CORE_DECLARE_ACTION (update_windows_dependencies)

                virtual const char *GetGroup () const {
                    return GROUP_WINDOWS;
                }

                virtual void PrintHelp (std::ostream &stream) const {
                    stream <<
                        "-a:" << GetName () << " path\n\n"
                        "a - Convert windows dependency list to cygwin path.\n"
                        "path - Path to windows dependency output.\n";
                }

                virtual void Execute  () {
                    std::string dependent;
                    std::string dependency;
                    std::set<std::string> dependencies;
                    {
                        std::fstream file (Options::Instance ().path.c_str (), std::fstream::in);
                        if (file.is_open ()) {
                            if (std::getline (file, dependent)) {
                                dependent = util::TrimSpaces (dependent.c_str ());
                                std::size_t index = dependent.size ();
                                while (index > 0 &&
                                        (dependent[index - 1] == '\\' ||
                                            isspace (dependent[index - 1]) ||
                                            dependent[index - 1] == ':')) {
                                    --index;
                                }
                                dependent = dependent.substr (0, index);
                                for (std::string line; std::getline (file, line);) {
                                    line = util::TrimSpaces (line.c_str ());
                                    std::size_t index = line.size ();
                                    while (index > 0 && (line[index - 1] == '\\' || isspace (line[index - 1]))) {
                                        --index;
                                    }
                                    line = line.substr (0, index);
                                    if (!line.empty ()) {
                                        if (dependency.empty ()) {
                                            dependency = line;
                                        }
                                        else {
                                            dependencies.insert (
                                                core::CygwinMountTable::Instance ().ToCygwinPath (line));
                                        }
                                    }
                                }
                            }
                        }
                    }
                    WriteDependencies (
                        dependent,
                        dependency,
                        dependencies,
                        Options::Instance ().path);
                }

            private:
                void WriteDependencies (
                        const std::string &dependent,
                        const std::string &dependency,
                        const std::set<std::string> &dependencies,
                        const std::string &path) {
                    std::fstream file (path.c_str (), std::fstream::out | std::fstream::trunc);
                    if (file.is_open ()) {
                        file << dependent.c_str () << ": \\\n " << dependency;
                        for (std::set<std::string>::const_iterator
                                it = dependencies.begin (),
                                end = dependencies.end (); it != end; ++it) {
                            file << " \\\n " << *it;
                        }
                        file << "\n";
                        for (std::set<std::string>::const_iterator
                                it = dependencies.begin (),
                                end = dependencies.end (); it != end; ++it) {
                            file << *it << ":\n";
                        }
                    }
                    else {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                            "Unable to open %s.", path.c_str ());
                    }
                }
            };

            THEKOGANS_MAKE_CORE_IMPLEMENT_ACTION (update_windows_dependencies)
        }

    } // namespace make
} // namespace thekogans
