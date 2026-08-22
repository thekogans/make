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
#include "thekogans/make/core/Installer.h"
#include "thekogans/make/Options.h"
#include "thekogans/make/actions/install_plugin_hosts.h"

namespace thekogans {
    namespace make {

        THEKOGANS_UTIL_IMPLEMENT_DYNAMIC_CREATABLE (install_plugin_hosts, Action::TYPE)

        void install_plugin_hosts::PrintHelp (std::ostream &stream) const {
            stream <<
                "-a:" << Type () << " [-c:[" CONFIG_DEBUG " | " CONFIG_RELEASE "]] "
                "[-w:[yes | no]] [-x:[yes | no]] path\n\n"
                "a - Install the given plugin hosts.\n"
                "c - Build configuration [" CONFIG_DEBUG " | " CONFIG_RELEASE "].\n"
                "w - Hide commands [yes | no].\n"
                "x - Parallel build [yes | no].\n"
                "path - Path to " THEKOGANS_MAKE_XML " file.\n";
        }

        void install_plugin_hosts::Execute () {
            core::Installer installer (
                Options::Instance ()->config,
                Options::Instance ()->type,
                Options::Instance ()->hide_commands,
                Options::Instance ()->parallel_build);
            installer.InstallPluginHosts (Options::Instance ()->path);
        }

    } // namespace make
} // namespace thekogans
