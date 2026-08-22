// Copyright 2016 Boris Kogan (boris@thekogans.net)
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

#if defined (THEKOGANS_MAKE_TYPE_Static)
    #include "thekogans/util/Environment.h"
    #include "thekogans/make/generators/make.cpp
#if defined (TOOLCHAIN_OS_Windows)
    #include "thekogans/make/generators/vs2013.cpp
    #include "thekogans/make/generators/vs2015.cpp
    #include "thekogans/make/generators/vs2017.cpp
    #include "thekogans/make/generators/vs2019.cpp
    #include "thekogans/make/generators/vsBase.cpp
#elif defined (TOOLCHAIN_OS_OSX)
    #include "thekogans/make/generators/Xcode.mm
#endif // defined (TOOLCHAIN_OS_Windows)
#endif // defined (THEKOGANS_MAKE_TYPE_Static)

namespace thekogans {
    namespace make {
        namespace generators {

        #if defined (THEKOGANS_MAKE_TYPE_Static)
            void Generators::StaticInit () {
                make::StaticInit ();
            #if defined (TOOLCHAIN_OS_Windows)
                vs2013::StaticInit ();
                vs2015::StaticInit ();
                vs2017::StaticInit ();
                vs2019::StaticInit ();
            #elif defined (TOOLCHAIN_OS_OSX)
                Xcode::StaticInit ();
            #endif // defined (TOOLCHAIN_OS_Windows)
            }
        #endif // defined (THEKOGANS_MAKE_TYPE_Static)

        } // namespace generators
    } // namespace make
} // namespace thekogans
