// Copyright 2011 Boris Kogan (boris@thekogans.net)
//
// This file is part of libthekogans_stream.
//
// libthekogans_stream is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libthekogans_stream is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libthekogans_stream. If not, see <http://www.gnu.org/licenses/>.

#if !defined (__thekogans_make_Action_h)
#define __thekogans_make_Action_h

#include <memory>
#include <string>
#include <map>
#include <iostream>
#include <list>

namespace thekogans {
    namespace make {

        // actions:

        // build
        //   create_build_system
        //   delete_build_system
        //   build_project

        // windows
        //   update_cl_dependencies
        //   update_windows_dependencies
        //   list_cygwin_mount_table

        // install
        //   install_library
        //   install_program
        //   install_plugin
        //   install_plugin_hosts
        //   uninstall_library
        //   uninstall_program
        //   uninstall_plugin
        //   copy_dependencies
        //   copy_plugin

        // toolchain Sources.xml
        //   add_toolchain_source
        //   delete_toolchain_source
        //   list_toolchain_sources
        //   update_toolchain_sources
        //   get_toolchain_source_url
        //   get_toolchain_source_project_latest_version
        //   get_toolchain_source_project_versions
        //   get_toolchain_source_project_sha_256
        //   get_toolchain_source_toolchain_latest_version
        //   get_toolchain_source_toolchain_versions
        //   get_toolchain_source_toolchain_file
        //   get_toolchain_source_toolchain_sha_256

        // sources
        //   create_source
        //   cleanup_source
        //   add_source_project
        //   delete_source_project
        //   cleanup_source_project
        //   add_source_toolchain
        //   delete_source_toolchain
        //   cleanup_source_toolchain
        //   get_source_url
        //   get_source_project_latest_version
        //   get_source_project_versions
        //   get_source_project_sha_256
        //   get_source_toolchain_latest_version
        //   get_source_toolchain_versions
        //   get_source_toolchain_file
        //   get_source_toolchain_sha_256

        // thekogans_make.xml
        //   get_schema_version
        //   check_dependencies
        //   list_dependencies
        //   get_project_version
        //   get_project_type
        //   get_naming_convention
        //   get_build_config
        //   get_build_type
        //   get_project_dependency_version
        //   get_toolchain_dependency_version
        //   get_toolchain_versions

        // toolchain
        //   cleanup_toolchain

        struct Action {
            /// \brief
            /// Convenient typedef for std::shared_ptr<Action>.
            typedef std::unique_ptr<Action> UniquePtr;

            /// \brief
            /// typedef for the Action factory function.
            typedef UniquePtr (*Factory) ();
            /// \brief
            /// typedef for the Action map.
            typedef std::map<std::string, Factory> Map;
            /// \brief
            /// Used for Action dynamic discovery and creation.
            /// \param[in] type Action type (it's name).
            /// \return A Action based on the passed in type.
            static UniquePtr Get (const std::string &type);
            /// \struct MapInitializer Action.h thekogans/make/Action.h
            ///
            /// \brief
            /// MapInitializer is used to initialize the Action::map.
            /// It should not be used directly, and instead is included
            /// in THEKOGANS_MAKE_DECLARE_ACTION/THEKOGANS_MAKE_IMPLEMENT_ACTION.
            /// If you are deriving a action from Action, and you want
            /// it to be dynamically discoverable/creatable, add
            /// THEKOGANS_MAKE_DECLARE_ACTION to it's declaration,
            /// and THEKOGANS_MAKE_IMPLEMENT_ACTION to it's definition.
            struct MapInitializer {
                /// \brief
                /// ctor. Add action of type, and factory for creating it
                /// to the Action::map
                /// \param[in] type Action type (it's class name).
                /// \param[in] factory Action creation factory.
                MapInitializer (
                    const std::string &type,
                    Factory factory);
            };
            /// \brief
            /// Get the list of all actions registered with the map.
            /// \param[out] actions List of registered generetors.
            static void GetActions (std::list<std::string> &actions);

            static void GetGroups (std::list<std::string> &groups);
            static void GetGroupActions (
                const std::string &group,
                std::list<std::string> &groups);

            /// \brief
            /// Virtual dtor.
            virtual ~Action () {}

            /// \brief
            /// Return the class name of the action.
            /// \return Class name of the action.
            virtual const char *GetName () const = 0;

            static const char * const GROUP_BUILD;
            static const char * const GROUP_WINDOWS;
            static const char * const GROUP_INSTALL;
            static const char * const GROUP_TOOLCHAIN_SOURCES_XML;
            static const char * const GROUP_SOURCES;
            static const char * const GROUP_THEKOGANS_MAKE_XML;
            static const char * const GROUP_TOOLCHAIN;

            /// \brief
            /// Return the group the action belongs to. Used to 'group' actions hierarchically.
            /// \return Action group name.
            virtual const char *GetGroup () const {
                return "";
            }

            /// \brief
            virtual void PrintHelp (std::ostream &stream) const = 0;

            /// \brief
            virtual void Execute  () = 0;
        };

        /// \brief
        /// Dynamic discovery macro. Add this to your class declaration.
        /// Example:
        /// \code{.cpp}
        /// struct create_build_system : public Action {
        ///     THEKOGANS_MAKE_DECLARE_ACTION (create_build_system)
        ///     ...
        /// };
        /// \endcode
        #define THEKOGANS_MAKE_DECLARE_ACTION(type)\
        public:\
            static thekogans::make::Action::MapInitializer mapInitializer;\
            static thekogans::make::Action::UniquePtr Create () {\
                return thekogans::make::Action::UniquePtr (new type);\
            }\
            virtual const char *GetName () const {\
                return #type;\
            }

        /// \brief
        /// Dynamic discovery macro. Instantiate one of these in the class cpp file.
        /// Example:
        /// \code{.cpp}
        /// THEKOGANS_MAKE_IMPLEMENT_ACTION (create_build_system)
        /// \endcode
        #define THEKOGANS_MAKE_IMPLEMENT_ACTION(type)\
            thekogans::make::Action::MapInitializer type::mapInitializer (\
                #type, type::Create);

        inline std::ostream &operator << (
                std::ostream &stream,
                const Action &action) {
            action.PrintHelp (stream);
            return stream;
        }

    } // namespace make
} // namespace thekogans

#endif // !defined (__thekogans_make_Action_h)
