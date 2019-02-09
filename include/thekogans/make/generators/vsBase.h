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

#if !defined (__thekogans_make_vsBase_h)
#define __thekogans_make_vsBase_h

#include <string>
#include <list>
#include <set>
#include "thekogans/util/Types.h"
#include "thekogans/util/GUID.h"
#include "thekogans/make/core/Generator.h"
#include "thekogans/make/core/thekogans_make.h"

namespace thekogans {
    namespace make {

        struct vsBase : public core::Generator {
        protected:
            const char *name;
            bool rootProject;
            std::string defaultOrganization;
            typedef std::pair<std::string, std::string> FileAndFilter;
            std::list<FileAndFilter> masm_headers;
            std::list<FileAndFilter> masm_sources;
            std::list<FileAndFilter> nasm_headers;
            std::list<FileAndFilter> nasm_sources;
            std::list<FileAndFilter> c_headers;
            std::list<FileAndFilter> c_sources;
            std::list<FileAndFilter> cpp_headers;
            std::list<FileAndFilter> cpp_sources;
            std::list<FileAndFilter> rc_sources;
            std::list<FileAndFilter> resources;
            std::list<FileAndFilter> custom_build_sources;
            std::set<std::string> header_filters;
            std::set<std::string> source_filters;
            std::set<std::string> rc_source_filters;
            std::set<std::string> resource_filters;
            std::set<std::string> custom_build_filters;

        public:
            vsBase (
                const char *name_ = "vsBase",
                bool rootProject_ = true,
                const std::string &defaultOrganization_ = std::string ()) :
                name (name_),
                rootProject (rootProject_),
                defaultOrganization (defaultOrganization_) {}

            virtual const char *GetName () const {
                return name;
            }

            virtual std::string slnGetFormatVersion () const = 0;
            virtual std::string slnGetVisualStudio () const = 0;

            virtual std::string vcxprojGetToolsVersion () const = 0;
            virtual std::string vcxprojGetPlatformToolset () const = 0;

            virtual std::string vcxprojfiltersGetToolsVersion () const = 0;

            // Generator
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

        protected:
            void sln (const core::thekogans_make &thekogans_make);
            void vcxproj (const core::thekogans_make &thekogans_make);
            void vcxprojfilters (const core::thekogans_make &thekogans_make);

        private:
            void AddCustomBuildRule (
                const core::thekogans_make &thekogans_make,
                const core::thekogans_make::FileList::File &file,
                const std::string &prefix);
            std::string GetFilter (
                const std::string &filterBase,
                const std::string &path,
                std::set<std::string> &filters);
            std::string GetMasmCommandLine (const core::thekogans_make &thekogans_make) const;
            std::string GetNasmCommandLine (const core::thekogans_make &thekogans_make) const;
            std::string GetRCPreprocessorDefinitions (const core::thekogans_make &thekogans_make) const;
            std::string GetRCIncludeDirectories (const core::thekogans_make &thekogans_make) const;
            std::string GetQualifiedName (
                const std::string &organization,
                const std::string &project) const;
        };

    } // namespace make
} // namespace thekogans

#endif // !defined (__thekogans_make_vsBase_h)
