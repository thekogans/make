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

#if !defined (__thekogans_make_vs2019_h)
#define __thekogans_make_vs2019_h

#include <string>
#include "thekogans/make/generators/vsBase.h"

namespace thekogans {
    namespace make {

        struct vs2019 : public vsBase {
            THEKOGANS_MAKE_CORE_DECLARE_GENERATOR (vs2019)

            vs2019 (bool rootProject) :
                vsBase (rootProject) {}

            virtual std::string slnGetFormatVersion () const {
                return "12.00";
            }
            virtual std::string slnGetVisualStudio () const {
                return "16";
            }

            virtual std::string vcxprojGetToolsVersion () const {
                return "16.0";
            }
            virtual std::string vcxprojGetPlatformToolset () const {
                return "v142";
            }

            virtual std::string vcxprojfiltersGetToolsVersion () const {
                return "4.0";
            }
        };

    } // namespace make
} // namespace thekogans

#endif // !defined (__thekogans_make_vs2019_h)
