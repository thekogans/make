// Copyright 2016 Boris Kogan (boris@thekogans.net)
//
// This file is part of libthekogans_make.
//
// libthekogans_make is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libthekogans_make is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libthekogans_make. If not, see <http://www.gnu.org/licenses/>.

#if !defined (__thekogans_make_actions_Actions_h)
#define __thekogans_make_actions_Actions_h

namespace thekogans {
    namespace make {
        namespace actions {

        #if defined (THEKOGANS_MAKE_TYPE_Static)
            /// \struct Actions Actions.h thekogans/make/actions/Actions.h
            ///
            /// \brief
            /// Actions collects all actions in one convenient place. It exposes
            /// a StaticInit method to register them all for discovery.
            struct Actions {
                /// \brief
                /// Because Make uses dynamic initialization, when using it in
                /// static builds call this method to have it explicitly include
                /// all known internal actions.
                static void StaticInit ();
            };
        #endif // defined (THEKOGANS_MAKE_TYPE_Static)

        } // namespace actions
    } // namespace make
} // namespace thekogans

#endif // !defined (__thekogans_make_actions_Actions_h)
