# Copyright 2011 Boris Kogan (boris@thekogans.net)
#
# This file is part of thekogans_make.
#
# thekogans_make is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# thekogans_make is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with thekogans_make. If not, see <http://www.gnu.org/licenses/>.

# Makefile for thekogans_make.
# Please see $(TOOLCHAIN_ROOT)/common/resources/make.rules.[top | bottom]

organization := thekogans
project := make
project_type := program
config := Release
type := Static
naming_convention := $(TOOLCHAIN_NAMING_CONVENTION)

include $(TOOLCHAIN_ROOT)/common/resources/make.rules.top

util_version := $(util_major_version).$(util_minor_version).$(util_patch_version)
make_core_version := $(make_core_major_version).$(make_core_minor_version).$(make_core_patch_version)

include_directories :=\
  $(project_root)/include\
  $(DEVELOPMENT_ROOT)/thekogans/make/core-$(make_core_version)/include\
  $(DEVELOPMENT_ROOT)/thekogans/make/core-$(make_core_version)/include/thekogans/make/core\
  $(DEVELOPMENT_ROOT)/thekogans/util-$(util_version)/include\
  $(DEVELOPMENT_ROOT)/thekogans/util-$(util_version)/include/3rdparty\
  $(DEVELOPMENT_ROOT)/thekogans/util-$(util_version)/include/3rdparty/private\
  $(DEVELOPMENT_ROOT)/thekogans/util-$(util_version)/include/3rdparty/private/zlib

ifeq "$(TOOLCHAIN_OS)" "Windows"
  link_libraries += Ws2_32.lib Iphlpapi.lib mpr.lib Wtsapi32.lib
else
  ifeq "$(TOOLCHAIN_OS)" "Linux"
    link_libraries += -lpthread -lrt -ldl
  else
    ifeq "$(TOOLCHAIN_OS)" "OSX"
      link_libraries +=\
        -lpthread\
        -framework Foundation\
        -framework Security\
        -framework Cocoa\
        -framework CoreFoundation\
        -framework CoreServices\
        -framework SystemConfiguration\
        -framework IOKit
    endif
  endif
endif

common_preprocessor_definitions :=\
  TOOLCHAIN_OS_$(TOOLCHAIN_OS)\
  TOOLCHAIN_ARCH_$(TOOLCHAIN_ARCH)\
  TOOLCHAIN_COMPILER_$(TOOLCHAIN_COMPILER)\
  TOOLCHAIN_ENDIAN_$(TOOLCHAIN_ENDIAN)\
  TOOLCHAIN_CONFIG_$(config)\
  TOOLCHAIN_TYPE_$(type)\
  TOOLCHAIN_TRIPLET="$(TOOLCHAIN_TRIPLET)"\
  THEKOGANS_UTIL_MAJOR_VERSION=$(util_major_version)\
  THEKOGANS_UTIL_MINOR_VERSION=$(util_minor_version)\
  THEKOGANS_UTIL_PATCH_VERSION=$(util_patch_version)\
  THEKOGANS_MAKE_CORE_MAJOR_VERSION=$(make_core_major_version)\
  THEKOGANS_MAKE_CORE_MINOR_VERSION=$(make_core_minor_version)\
  THEKOGANS_MAKE_CORE_PATCH_VERSION=$(make_core_patch_version)\
  THEKOGANS_MAKE_MAJOR_VERSION=$(major_version)\
  THEKOGANS_MAKE_MINOR_VERSION=$(minor_version)\
  THEKOGANS_MAKE_PATCH_VERSION=$(patch_version)\
  THEKOGANS_MAKE_CONFIG_$(config)\
  THEKOGANS_MAKE_TYPE_$(type)\
  THEKOGANS_UTIL_CONFIG_$(config)\
  THEKOGANS_UTIL_TYPE_$(type)\
  THEKOGANS_MAKE_CORE_CONFIG_$(config)\
  THEKOGANS_MAKE_CORE_TYPE_$(type)
ifeq "$(TOOLCHAIN_OS)" "Windows"
  common_preprocessor_definitions +=\
    _CRT_SECURE_NO_WARNINGS
else
  common_preprocessor_definitions +=\
	THEKOGANS_UTIL_HAVE_MMAP
endif

cpp_headers :=\
  $(project_root)/include/$(organization)/$(project)/Action.h\
  $(project_root)/include/$(organization)/$(project)/Options.h\
  $(project_root)/include/$(organization)/$(project)/Version.h
# actions
cpp_headers +=\
  $(project_root)/include/$(organization)/$(project)/actions/Actions.h\
  $(project_root)/include/$(organization)/$(project)/actions/add_source_project.h\
  $(project_root)/include/$(organization)/$(project)/actions/add_source_toolchain.h\
  $(project_root)/include/$(organization)/$(project)/actions/build_project.h\
  $(project_root)/include/$(organization)/$(project)/actions/check_dependencies.h\
  $(project_root)/include/$(organization)/$(project)/actions/cleanup_source_project.h\
  $(project_root)/include/$(organization)/$(project)/actions/cleanup_source_toolchain.h\
  $(project_root)/include/$(organization)/$(project)/actions/cleanup_source.h\
  $(project_root)/include/$(organization)/$(project)/actions/cleanup_toolchain.h\
  $(project_root)/include/$(organization)/$(project)/actions/copy_dependencies.h\
  $(project_root)/include/$(organization)/$(project)/actions/copy_plugin.h\
  $(project_root)/include/$(organization)/$(project)/actions/create_build_system.h\
  $(project_root)/include/$(organization)/$(project)/actions/create_source.h\
  $(project_root)/include/$(organization)/$(project)/actions/delete_build_system.h\
  $(project_root)/include/$(organization)/$(project)/actions/delete_source_project.h\
  $(project_root)/include/$(organization)/$(project)/actions/delete_source_toolchain.h\
  $(project_root)/include/$(organization)/$(project)/actions/delete_toolchain_source.h\
  $(project_root)/include/$(organization)/$(project)/actions/destroy_source.h\
  $(project_root)/include/$(organization)/$(project)/actions/get_build_config.h\
  $(project_root)/include/$(organization)/$(project)/actions/get_build_type.h\
  $(project_root)/include/$(organization)/$(project)/actions/get_naming_convention.h\
  $(project_root)/include/$(organization)/$(project)/actions/get_project_dependency_version.h\
  $(project_root)/include/$(organization)/$(project)/actions/get_project_type.h\
  $(project_root)/include/$(organization)/$(project)/actions/get_project_version.h\
  $(project_root)/include/$(organization)/$(project)/actions/get_schema_version.h\
  $(project_root)/include/$(organization)/$(project)/actions/get_source_project_latest_version.h\
  $(project_root)/include/$(organization)/$(project)/actions/get_source_project_sha2_256.h\
  $(project_root)/include/$(organization)/$(project)/actions/get_source_project_versions.h\
  $(project_root)/include/$(organization)/$(project)/actions/get_source_toolchain_file.h\
  $(project_root)/include/$(organization)/$(project)/actions/get_source_toolchain_latest_version.h\
  $(project_root)/include/$(organization)/$(project)/actions/get_source_toolchain_sha2_256.h\
  $(project_root)/include/$(organization)/$(project)/actions/get_source_toolchain_versions.h\
  $(project_root)/include/$(organization)/$(project)/actions/get_source_url.h\
  $(project_root)/include/$(organization)/$(project)/actions/get_toolchain_dependency_version.h\
  $(project_root)/include/$(organization)/$(project)/actions/get_toolchain_source_project_latest_version.h\
  $(project_root)/include/$(organization)/$(project)/actions/get_toolchain_source_project_sha2_256.h\
  $(project_root)/include/$(organization)/$(project)/actions/get_toolchain_source_project_versions.h\
  $(project_root)/include/$(organization)/$(project)/actions/get_toolchain_source_toolchain_file.h\
  $(project_root)/include/$(organization)/$(project)/actions/get_toolchain_source_toolchain_latest_version.h\
  $(project_root)/include/$(organization)/$(project)/actions/get_toolchain_source_toolchain_sha2_256.h\
  $(project_root)/include/$(organization)/$(project)/actions/get_toolchain_source_toolchain_versions.h\
  $(project_root)/include/$(organization)/$(project)/actions/get_toolchain_source_url.h\
  $(project_root)/include/$(organization)/$(project)/actions/get_toolchain_versions.h\
  $(project_root)/include/$(organization)/$(project)/actions/install_library.h\
  $(project_root)/include/$(organization)/$(project)/actions/install_plugin_hosts.h\
  $(project_root)/include/$(organization)/$(project)/actions/install_plugin.h\
  $(project_root)/include/$(organization)/$(project)/actions/install_program.h\
  $(project_root)/include/$(organization)/$(project)/actions/list_dependencies.h\
  $(project_root)/include/$(organization)/$(project)/actions/list_toolchain_sources.h\
  $(project_root)/include/$(organization)/$(project)/actions/uninstall_library.h\
  $(project_root)/include/$(organization)/$(project)/actions/uninstall_plugin.h\
  $(project_root)/include/$(organization)/$(project)/actions/uninstall_program.h
ifdef THEKOGANS_MAKE_HAVE_CURL
  cpp_headers +=\
    $(project_root)/include/$(organization)/$(project)/actions/add_toolchain_source.h\
    $(project_root)/include/$(organization)/$(project)/actions/update_toolchain_sources.h
endif
ifeq "$(TOOLCHAIN_OS)" "Windows"
    cpp_headers +=\
	  $(project_root)/include/$(organization)/$(project)/actions/list_cygwin_mount_table.h\
      $(project_root)/include/$(organization)/$(project)/actions/update_cl_dependencies.h\
      $(project_root)/include/$(organization)/$(project)/actions/update_windows_dependencies.h
endif
# generators
cpp_headers +=\
  $(project_root)/include/$(organization)/$(project)/generators/Generators.h\
  $(project_root)/include/$(organization)/$(project)/generators/make.h
ifeq "$(TOOLCHAIN_OS)" "Windows"
  cpp_headers +=\
    $(project_root)/include/$(organization)/$(project)/generators/vsBase.h\
    $(project_root)/include/$(organization)/$(project)/generators/vs2013.h\
    $(project_root)/include/$(organization)/$(project)/generators/vs2015.h\
    $(project_root)/include/$(organization)/$(project)/generators/vs2017.h\
    $(project_root)/include/$(organization)/$(project)/generators/vs2019.h
else
  ifeq "$(TOOLCHAIN_OS)" "OSX"
    cpp_headers +=\
      $(project_root)/include/$(organization)/$(project)/generators/Xcode.h
  endif
endif
# functions
cpp_headers +=\
  $(project_root)/include/$(organization)/$(project)/functions/Functions.h\
  $(project_root)/include/$(organization)/$(project)/functions/dependency_have_feature.h\
  $(project_root)/include/$(organization)/$(project)/functions/get_directory.h\
  $(project_root)/include/$(organization)/$(project)/functions/get_directory_name.h\
  $(project_root)/include/$(organization)/$(project)/functions/get_extension.h\
  $(project_root)/include/$(organization)/$(project)/functions/get_file_name.h\
  $(project_root)/include/$(organization)/$(project)/functions/get_full_file_name.h\
  $(project_root)/include/$(organization)/$(project)/functions/get_project_root.h\
  $(project_root)/include/$(organization)/$(project)/functions/get_project_version.h\
  $(project_root)/include/$(organization)/$(project)/functions/get_project_program.h\
  $(project_root)/include/$(organization)/$(project)/functions/get_project_library.h\
  $(project_root)/include/$(organization)/$(project)/functions/get_toolchain_program.h\
  $(project_root)/include/$(organization)/$(project)/functions/get_toolchain_library.h\
  $(project_root)/include/$(organization)/$(project)/functions/get_toolchain_include_dir.h\
  $(project_root)/include/$(organization)/$(project)/functions/get_toolchain_lib_dir.h\
  $(project_root)/include/$(organization)/$(project)/functions/get_toolchain_latest_version.h\
  $(project_root)/include/$(organization)/$(project)/functions/get_major_version.h\
  $(project_root)/include/$(organization)/$(project)/functions/get_minor_version.h\
  $(project_root)/include/$(organization)/$(project)/functions/get_patch_version.h\
  $(project_root)/include/$(organization)/$(project)/functions/have_feature.h\
  $(project_root)/include/$(organization)/$(project)/functions/is_defined.h\
  $(project_root)/include/$(organization)/$(project)/functions/is_installed.h\
  $(project_root)/include/$(organization)/$(project)/functions/make_path.h\
  $(project_root)/include/$(organization)/$(project)/functions/path_exists.h\
  $(project_root)/include/$(organization)/$(project)/functions/to_build_system_path.h\
  $(project_root)/include/$(organization)/$(project)/functions/to_project_build_path.h\
  $(project_root)/include/$(organization)/$(project)/functions/to_project_path.h\
  $(project_root)/include/$(organization)/$(project)/functions/to_system_path.h

# zlib
c_sources :=\
  $(wildcard $(DEVELOPMENT_ROOT)/thekogans/util-$(util_version)/src/3rdparty/zlib/*.c)

# util
cpp_sources :=\
  $(wildcard $(DEVELOPMENT_ROOT)/thekogans/util-$(util_version)/src/*.cpp)\
  $(wildcard $(DEVELOPMENT_ROOT)/thekogans/util-$(util_version)/src/3rdparty/boost/atomic/*.cpp)\
  $(wildcard $(DEVELOPMENT_ROOT)/thekogans/util-$(util_version)/src/3rdparty/pugixml/pugixml.cpp)
ifeq "$(TOOLCHAIN_OS)" "Windows"
  cpp_sources +=\
    $(wildcard $(DEVELOPMENT_ROOT)/thekogans/util-$(util_version)/src/os/windows/*.cpp)
else
  ifeq "$(TOOLCHAIN_OS)" "Linux"
    cpp_sources +=\
      $(wildcard $(DEVELOPMENT_ROOT)/thekogans/util-$(util_version)/src/os/linux/*.cpp)
  else
    ifeq "$(TOOLCHAIN_OS)" "OSX"
      cpp_sources +=\
        $(wildcard $(DEVELOPMENT_ROOT)/thekogans/util-$(util_version)/src/os/osx/*.cpp)
    endif
  endif
endif
# make_core
cpp_sources +=\
  $(wildcard $(DEVELOPMENT_ROOT)/thekogans/make/core-$(make_core_version)/src/*.cpp)
# make
cpp_sources +=\
  $(project_root)/src/Action.cpp\
  $(project_root)/src/Options.cpp\
  $(project_root)/src/Version.cpp\
  $(project_root)/src/main.cpp
# actions
cpp_sources +=\
  $(project_root)/src/actions/Actions.cpp\
  $(project_root)/src/actions/add_source_project.cpp\
  $(project_root)/src/actions/add_source_toolchain.cpp\
  $(project_root)/src/actions/build_project.cpp\
  $(project_root)/src/actions/check_dependencies.cpp\
  $(project_root)/src/actions/cleanup_source_project.cpp\
  $(project_root)/src/actions/cleanup_source_toolchain.cpp\
  $(project_root)/src/actions/cleanup_source.cpp\
  $(project_root)/src/actions/cleanup_toolchain.cpp\
  $(project_root)/src/actions/copy_dependencies.cpp\
  $(project_root)/src/actions/copy_plugin.cpp\
  $(project_root)/src/actions/create_build_system.cpp\
  $(project_root)/src/actions/create_source.cpp\
  $(project_root)/src/actions/delete_build_system.cpp\
  $(project_root)/src/actions/delete_source_project.cpp\
  $(project_root)/src/actions/delete_source_toolchain.cpp\
  $(project_root)/src/actions/delete_toolchain_source.cpp\
  $(project_root)/src/actions/destroy_source.cpp\
  $(project_root)/src/actions/get_build_config.cpp\
  $(project_root)/src/actions/get_build_type.cpp\
  $(project_root)/src/actions/get_naming_convention.cpp\
  $(project_root)/src/actions/get_project_dependency_version.cpp\
  $(project_root)/src/actions/get_project_type.cpp\
  $(project_root)/src/actions/get_project_version.cpp\
  $(project_root)/src/actions/get_schema_version.cpp\
  $(project_root)/src/actions/get_source_project_latest_version.cpp\
  $(project_root)/src/actions/get_source_project_sha2_256.cpp\
  $(project_root)/src/actions/get_source_project_versions.cpp\
  $(project_root)/src/actions/get_source_toolchain_file.cpp\
  $(project_root)/src/actions/get_source_toolchain_latest_version.cpp\
  $(project_root)/src/actions/get_source_toolchain_sha2_256.cpp\
  $(project_root)/src/actions/get_source_toolchain_versions.cpp\
  $(project_root)/src/actions/get_source_url.cpp\
  $(project_root)/src/actions/get_toolchain_dependency_version.cpp\
  $(project_root)/src/actions/get_toolchain_source_project_latest_version.cpp\
  $(project_root)/src/actions/get_toolchain_source_project_sha2_256.cpp\
  $(project_root)/src/actions/get_toolchain_source_project_versions.cpp\
  $(project_root)/src/actions/get_toolchain_source_toolchain_file.cpp\
  $(project_root)/src/actions/get_toolchain_source_toolchain_latest_version.cpp\
  $(project_root)/src/actions/get_toolchain_source_toolchain_sha2_256.cpp\
  $(project_root)/src/actions/get_toolchain_source_toolchain_versions.cpp\
  $(project_root)/src/actions/get_toolchain_source_url.cpp\
  $(project_root)/src/actions/get_toolchain_versions.cpp\
  $(project_root)/src/actions/install_library.cpp\
  $(project_root)/src/actions/install_plugin_hosts.cpp\
  $(project_root)/src/actions/install_plugin.cpp\
  $(project_root)/src/actions/install_program.cpp\
  $(project_root)/src/actions/list_dependencies.cpp\
  $(project_root)/src/actions/list_toolchain_sources.cpp\
  $(project_root)/src/actions/uninstall_library.cpp\
  $(project_root)/src/actions/uninstall_plugin.cpp\
  $(project_root)/src/actions/uninstall_program.cpp
ifdef THEKOGANS_MAKE_HAVE_CURL
  cpp_sources +=\
    $(project_root)/src/actions/add_toolchain_source.cpp\
    $(project_root)/src/actions/update_toolchain_sources.cpp
endif
ifeq "$(TOOLCHAIN_OS)" "Windows"
  cpp_sources +=\
      $(project_root)/src/actions/update_cl_dependencies.cpp\
      $(project_root)/src/actions/update_windows_dependencies.cpp\
      $(project_root)/src/actions/list_cygwin_mount_table.cpp
endif
# generators
cpp_sources +=\
  $(project_root)/src/generators/Generators.cpp\
  $(project_root)/src/generators/make.cpp
ifeq "$(TOOLCHAIN_OS)" "Windows"
  cpp_sources +=\
    $(project_root)/src/generators/vsBase.cpp\
    $(project_root)/src/generators/vs2013.cpp\
    $(project_root)/src/generators/vs2015.cpp\
    $(project_root)/src/generators/vs2017.cpp
endif
# functions
cpp_sources +=\
  $(project_root)/src/functions/Functions.cpp\
  $(project_root)/src/functions/dependency_have_feature.cpp\
  $(project_root)/src/functions/get_directory.cpp\
  $(project_root)/src/functions/get_directory_name.cpp\
  $(project_root)/src/functions/get_extension.cpp\
  $(project_root)/src/functions/get_file_name.cpp\
  $(project_root)/src/functions/get_full_file_name.cpp\
  $(project_root)/src/functions/get_project_root.cpp\
  $(project_root)/src/functions/get_project_version.cpp\
  $(project_root)/src/functions/get_project_program.cpp\
  $(project_root)/src/functions/get_project_library.cpp\
  $(project_root)/src/functions/get_toolchain_program.cpp\
  $(project_root)/src/functions/get_toolchain_library.cpp\
  $(project_root)/src/functions/get_toolchain_include_dir.cpp\
  $(project_root)/src/functions/get_toolchain_lib_dir.cpp\
  $(project_root)/src/functions/get_toolchain_latest_version.cpp\
  $(project_root)/src/functions/get_major_version.cpp\
  $(project_root)/src/functions/get_minor_version.cpp\
  $(project_root)/src/functions/get_patch_version.cpp\
  $(project_root)/src/functions/have_feature.cpp\
  $(project_root)/src/functions/is_defined.cpp\
  $(project_root)/src/functions/is_installed.cpp\
  $(project_root)/src/functions/make_path.cpp\
  $(project_root)/src/functions/path_exists.cpp\
  $(project_root)/src/functions/to_build_system_path.cpp\
  $(project_root)/src/functions/to_project_build_path.cpp\
  $(project_root)/src/functions/to_project_path.cpp\
  $(project_root)/src/functions/to_system_path.cpp

ifeq "$(TOOLCHAIN_OS)" "OSX"
  objective_cpp_sources :=\
	$(DEVELOPMENT_ROOT)/thekogans/util-$(util_version)/src/os/osx/OSXUtils.mm\
    $(project_root)/src/generators/Xcode.mm
endif

include $(TOOLCHAIN_ROOT)/common/resources/make.rules.bottom
