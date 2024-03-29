# Copyright 2022 David Feltell
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

#------------------------------------------------------------
# Install CPM package manager

set(CPM_DOWNLOAD_VERSION 0.34.0)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")

if (NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
	message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
	file(DOWNLOAD
		https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
		${CPM_DOWNLOAD_LOCATION}
		)
endif ()

include(${CPM_DOWNLOAD_LOCATION})


#------------------------------------------------------------
# Add project_options CMake library v0.21.0

CPMAddPackage("gh:cpp-best-practices/project_options@0.21.0")
include(${project_options_SOURCE_DIR}/src/DynamicProjectOptions.cmake)

#------------------------------------------------------------
# Begin project definition.

project(CppCAPI VERSION 1.0.0)

if (CPPCAPI_ENABLE_TESTS)
	# Tests install the project first, so default the install directory to be relative to the build
	# root. This must come before `dynamic_project_options()`
	if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND PROJECT_IS_TOP_LEVEL)
		set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/dist"
			CACHE PATH "Installation location" FORCE)
	endif ()
endif ()

# Too many false positives, especially with templated code.
set(ENABLE_CPPCHECK_DEFAULT OFF)
# Generate project_options and project_warnings INTERFACE targets.
dynamic_project_options()

set(CMAKE_CXX_STANDARD 17)

# For `generate_export_header`.
include(GenerateExportHeader)
# For `write_basic_package_version_file` et al.
include(CMakePackageConfigHelpers)
# For default install directories, e.g. CMAKE_INSTALL_LIBDIR.
include(GNUInstallDirs)
# Default to less spammy install status messages.
if (NOT DEFINED CMAKE_INSTALL_MESSAGE)
	set(CMAKE_INSTALL_MESSAGE LAZY)
endif ()


#------------------------------------------------------------
# Options

option(CPPCAPI_ENABLE_TESTS "Build/install tests and demos" OFF)


#------------------------------------------------------------
# Targets

# Convenience install target.

add_custom_target(
	cppcapi.install
	COMMAND ${CMAKE_COMMAND} --install ${PROJECT_BINARY_DIR}
)

# Plugin system

add_subdirectory(src)

# Demos/tests

if (CPPCAPI_ENABLE_TESTS)
	# Enable CTest target support (`add_test()`).
	enable_testing()
	add_subdirectory(tests)
endif ()


#------------------------------------------------------------
# CMake install package config

set(version_config_file "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config_file "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake")
set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

# Version fetched from the top level project()
write_basic_package_version_file(
	${version_config_file}
	COMPATIBILITY SameMinorVersion
)

configure_package_config_file(
	cmake/Config.cmake.in
	${project_config_file}
	INSTALL_DESTINATION ${config_install_dir}
)

install(
	EXPORT ${PROJECT_NAME}_EXPORTED_TARGETS
	DESTINATION ${config_install_dir}
	NAMESPACE ${PROJECT_NAME}::
	FILE ${PROJECT_NAME}Targets.cmake
)

install(
	FILES "${project_config_file}" "${version_config_file}"
	DESTINATION "${config_install_dir}"
)
