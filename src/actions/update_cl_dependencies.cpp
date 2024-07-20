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
#include "thekogans/util/os/windows/WindowsUtils.h"
#include "thekogans/make/core/Utils.h"
#include "thekogans/make/Options.h"
#include "thekogans/make/Action.h"

namespace thekogans {
    namespace make {

        namespace {
            #define strncasecmp _strnicmp

            struct update_cl_dependencies : public Action {
                THEKOGANS_MAKE_DECLARE_ACTION (update_cl_dependencies)

                virtual const char *GetGroup () const {
                    return GROUP_WINDOWS;
                }

                virtual void PrintHelp (std::ostream &stream) const {
                    stream <<
                        "-a:" << GetName () << " -d:'dependency path' path\n\n"
                        "a - Convert cl showIncludes output to make dependency list.\n"
                        "d - Dependent for which the conversion is being made (*.obj).\n"
                        "y - Dependency for which the conversion is being made (*.[c | cpp].\n"
                        "path - Path to cl showIncludes output.\n";
                }

                virtual void Execute  () {
                    if (!Options::Instance ()->dependency.empty ()) {
                        const std::string vsInstallDir =
                            core::CygwinMountTable::Instance ()->ToCygwinPath (
                                util::GetEnvironmentVariable ("VSINSTALLDIR"));
                        const std::string windowsSdkDir =
                            core::CygwinMountTable::Instance ()->ToCygwinPath (
                                util::GetEnvironmentVariable ("WindowsSdkDir"));
                        std::set<std::string> dependencies;
                        {
                            std::fstream file (Options::Instance ()->path.c_str (), std::fstream::in);
                            if (file.is_open ()) {
                                for (std::string line; std::getline (file, line);) {
                                    if (strncmp (line.c_str (), "Note: including file:", 21) == 0) {
                                        std::string dependencyPath =
                                            core::CygwinMountTable::Instance ()->ToCygwinPath (
                                                util::TrimSpaces (line.substr (21).c_str ()));
                                        if (!dependencyPath.empty () &&
                                            strncasecmp (dependencyPath.c_str (),
                                                vsInstallDir.c_str (), vsInstallDir.size ()) != 0 &&
                                            strncasecmp (dependencyPath.c_str (),
                                                windowsSdkDir.c_str (), windowsSdkDir.size ()) != 0) {
                                            dependencies.insert (dependencyPath);
                                        }
                                    }
                                }
                            }
                            else {
                                THEKOGANS_UTIL_THROW_STRING_EXCEPTION (
                                    "Unable to open '%s'.",
                                    Options::Instance ()->path.size ());
                            }
                        }
                        WriteDependencies (
                            Options::Instance ()->dependent,
                            Options::Instance ()->dependency,
                            dependencies,
                            Options::Instance ()->path);
                    }
                    else {
                        THEKOGANS_UTIL_THROW_STRING_EXCEPTION ("%s",
                            "No dependency (-d) specified.");
                    }
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

            THEKOGANS_MAKE_IMPLEMENT_ACTION (update_cl_dependencies)
        }

    } // namespace make
} // namespace thekogans
