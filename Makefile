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
  $(DEVELOPMENT_ROOT)/thekogans/util-$(util_version)/include/3rdparty

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
        -framework Cocoa\
        -framework CoreFoundation\
        -framework CoreServices\
        -framework SystemConfiguration
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
  THEKOGANS_UTIL_CONFIG_$(config)\
  THEKOGANS_UTIL_TYPE_$(type)\
  THEKOGANS_MAKE_CORE_CONFIG_$(config)\
  THEKOGANS_MAKE_CORE_TYPE_$(type)\
ifeq "$(TOOLCHAIN_OS)" "Windows"
  common_preprocessor_definitions +=\
    _CRT_SECURE_NO_WARNINGS
endif

cpp_headers :=\
  $(project_root)/include/$(organization)/$(project)/Action.h\
  $(project_root)/include/$(organization)/$(project)/Options.h\
  $(project_root)/include/$(organization)/$(project)/Version.h\
  $(project_root)/include/$(organization)/$(project)/generators/make.h
ifeq "$(TOOLCHAIN_OS)" "Windows"
  cpp_headers +=\
    $(project_root)/include/$(organization)/$(project)/generators/vsBase.h\
    $(project_root)/include/$(organization)/$(project)/generators/vs2013.h\
    $(project_root)/include/$(organization)/$(project)/generators/vs2015.h\
    $(project_root)/include/$(organization)/$(project)/generators/vs2017.h
else
  ifeq "$(TOOLCHAIN_OS)" "OSX"
    cpp_headers +=\
      $(project_root)/include/$(organization)/$(project)/generators/Xcode.h
  endif
endif
cpp_headers +=\
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
  $(project_root)/include/$(organization)/$(project)/functions/have_feature.h\
  $(project_root)/include/$(organization)/$(project)/functions/is_installed.h\
  $(project_root)/include/$(organization)/$(project)/functions/make_path.h\
  $(project_root)/include/$(organization)/$(project)/functions/to_system_path.h

cpp_sources :=\
  $(wildcard $(DEVELOPMENT_ROOT)/thekogans/util-$(util_version)/src/*.cpp)\
  $(wildcard $(DEVELOPMENT_ROOT)/thekogans/util-$(util_version)/src/3rdparty/boost/atomic/*.cpp)\
  $(wildcard $(DEVELOPMENT_ROOT)/thekogans/util-$(util_version)/src/3rdparty/pugixml/pugixml.cpp)\
  $(wildcard $(DEVELOPMENT_ROOT)/thekogans/make/core-$(make_core_version)/src/*.cpp)\
  $(project_root)/src/Action.cpp\
  $(project_root)/src/Options.cpp\
  $(project_root)/src/Version.cpp\
  $(project_root)/src/main.cpp\
  $(project_root)/src/actions/get_schema_version.cpp\
  $(project_root)/src/actions/create_build_system.cpp\
  $(project_root)/src/actions/delete_build_system.cpp\
  $(project_root)/src/actions/build_project.cpp
ifeq "$(TOOLCHAIN_OS)" "Windows"
  cpp_sources +=\
    $(project_root)/src/actions/update_cl_dependencies.cpp\
    $(project_root)/src/actions/update_windows_dependencies.cpp\
    $(project_root)/src/actions/list_cygwin_mount_table.cpp
endif
cpp_sources +=\
  $(project_root)/src/actions/install_library.cpp\
  $(project_root)/src/actions/install_program.cpp\
  $(project_root)/src/actions/install_plugin.cpp\
  $(project_root)/src/actions/install_plugin_hosts.cpp\
  $(project_root)/src/actions/uninstall_library.cpp\
  $(project_root)/src/actions/uninstall_program.cpp\
  $(project_root)/src/actions/uninstall_plugin.cpp\
  $(project_root)/src/actions/check_dependencies.cpp\
  $(project_root)/src/actions/list_dependencies.cpp\
  $(project_root)/src/actions/get_project_version.cpp\
  $(project_root)/src/actions/get_project_type.cpp\
  $(project_root)/src/actions/get_naming_convention.cpp\
  $(project_root)/src/actions/get_build_config.cpp\
  $(project_root)/src/actions/get_build_type.cpp\
  $(project_root)/src/actions/get_project_dependency_version.cpp\
  $(project_root)/src/actions/get_toolchain_dependency_version.cpp\
  $(project_root)/src/actions/get_toolchain_versions.cpp\
  $(project_root)/src/actions/cleanup_toolchain.cpp\
  $(project_root)/src/generators/make.cpp
ifeq "$(TOOLCHAIN_OS)" "Windows"
  cpp_sources +=\
    $(project_root)/src/generators/vsBase.cpp\
    $(project_root)/src/generators/vs2013.cpp\
    $(project_root)/src/generators/vs2015.cpp\
    $(project_root)/src/generators/vs2017.cpp
endif
cpp_sources +=\
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
  $(project_root)/src/functions/have_feature.cpp\
  $(project_root)/src/functions/is_installed.cpp\
  $(project_root)/src/functions/make_path.cpp\
  $(project_root)/src/functions/to_system_path.cpp

ifeq "$(TOOLCHAIN_OS)" "OSX"
  objective_cpp_sources :=\
	$(DEVELOPMENT_ROOT)/thekogans/util-$(util_version)/src/OSXUtils.mm\
    $(project_root)/src/generators/Xcode.mm
endif

include $(TOOLCHAIN_ROOT)/common/resources/make.rules.bottom
