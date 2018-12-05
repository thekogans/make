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

#if !defined (__thekogans_make_make_h)
#define __thekogans_make_make_h

#include <string>
#include "thekogans/make/core/Generator.h"

namespace thekogans {
    namespace make {

        struct make : public core::Generator {
            THEKOGANS_MAKE_CORE_DECLARE_GENERATOR (make)

        protected:
            bool rootProject;

        public:
            make (bool rootProject_ = true) :
                rootProject (rootProject_) {}

            /// \brief
            /// Generate a build system.
            /// \param[in] project_root Project root directory (where thekogans_make.xml resides).
            /// \param[in] config Debug or Release.
            /// \param[in] type Static or Shared.
            /// \param[in] generateDependencies true = Generate Dependencies.
            /// \param[in] force true = Don't bother checking the timestamps and force generation.
            /// \return true = Generated the build system, false = The build system was up to date.
            virtual bool Generate (
                const std::string &project_root,
                const std::string &config,
                const std::string &type,
                bool generateDependencies,
                bool force);

            /// \brief
            /// Delete a build system.
            /// \param[in] project_root Project root directory (where thekogans_make.xml resides).
            /// \param[in] config Debug or Release.
            /// \param[in] type Static or Shared.
            /// \param[in] deleteDependencies true = Delete Dependencies.
            virtual void Delete (
                const std::string &project_root,
                const std::string &config,
                const std::string &type,
                bool deleteDependencies);
        };

    } // namespace make
} // namespace thekogans

#endif // !defined (__thekogans_make_make_h)
