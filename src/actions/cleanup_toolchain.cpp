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

#include <set>
#include <map>
#include <iostream>
#include "thekogans/util/Path.h"
#include "thekogans/util/Version.h"
#include "thekogans/util/Directory.h"
#include "thekogans/make/core/Utils.h"
#include "thekogans/make/Options.h"
#include "thekogans/make/Action.h"

namespace thekogans {
    namespace make {

        namespace {
            struct cleanup_toolchain : public Action {
                THEKOGANS_UTIL_DECLARE_DYNAMIC_CREATABLE (cleanup_toolchain)

                virtual std::string GetGroup () const {
                    return GROUP_TOOLCHAIN;
                }

                virtual void PrintHelp (std::ostream &stream) const {
                    stream <<
                        "-a:" << Type () << " [-o:organization] [-p:project]\n\n"
                        "a - Delete old toolchain versions.\n"
                        "o - Optional organization name.\n"
                        "p - Optional project name.\n";
                }

                virtual void Execute  () {
                    std::string path =
                        ToSystemPath (core::MakePath (core::_TOOLCHAIN_DIR, core::CONFIG_DIR));
                    if (util::Path (path).Exists ()) {
                        typedef std::pair<std::string, std::string> Toolchain;
                        typedef std::set<util::Version, std::greater<util::Version>> Versions;
                        typedef std::map<Toolchain, Versions> ToolchainVersions;
                        ToolchainVersions toolchainVersions;
                        util::Directory directory (path);
                        util::Directory::Entry entry;
                        for (bool gotEntry = directory.GetFirstEntry (entry);
                                gotEntry; gotEntry = directory.GetNextEntry (entry)) {
                            if (entry.type == util::Directory::Entry::File) {
                                std::string organization;
                                std::string project;
                                std::string branch;
                                std::string version;
                                std::string ext;
                                if (core::ParseFileName (
                                        entry.name,
                                        organization,
                                        project,
                                        branch,
                                        version,
                                        ext) == 5 &&
                                        (Options::Instance ()->organization.empty () ||
                                            Options::Instance ()->organization == organization) &&
                                        (Options::Instance ()->project.empty () ||
                                            Options::Instance ()->project == project) &&
                                        ext == core::XML_EXT) {
                                    // Toolchain config files are branchless.
                                    assert (branch.empty ());
                                    toolchainVersions[
                                        Toolchain (organization, project)].insert (
                                            util::Version (version));
                                }
                            }
                        }
                        std::set<std::string> visitedDependencies;
                        for (ToolchainVersions::const_iterator
                                it = toolchainVersions.begin (),
                                end = toolchainVersions.end (); it != end; ++it) {
                            if (it->second.size () > 1) {
                                Versions::const_iterator jt = it->second.begin ();
                                Versions::const_iterator end = it->second.end ();
                                for (++jt; jt != end; ++jt) {
                                    core::Uninstall (
                                        it->first.first,
                                        it->first.second,
                                        (*jt).ToString (),
                                        false,
                                        visitedDependencies);
                                }
                            }
                        }
                    }
                }
            };

            THEKOGANS_UTIL_IMPLEMENT_DYNAMIC_CREATABLE (cleanup_toolchain, Action::TYPE)
        }

    } // namespace make
} // namespace thekogans
