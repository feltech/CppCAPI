cmake_minimum_required(VERSION 3.21)
project(FeltPluginSystem VERSION 1.0.0)

# For `generate_export_header`
include(GenerateExportHeader)
# For `write_basic_package_version_file` et al.
include(CMakePackageConfigHelpers)
# For default install directories, e.g. CMAKE_INSTALL_LIBDIR
include(GNUInstallDirs)
# Default to less spammy install status messages.
if (NOT DEFINED CMAKE_INSTALL_MESSAGE)
	set(CMAKE_INSTALL_MESSAGE LAZY)
endif ()

#------------------------------------------------------------
# Options

option(FELTPLUGIN_ENABLE_TESTS "Build/install tests and demos" OFF)


#------------------------------------------------------------
# Targets

# Plugin system

add_subdirectory(src)

# Demos/tests

if (FELTPLUGIN_ENABLE_TESTS)
	# Tests install the project first, so default the install directory to be relative to the build
	# root.
	if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND PROJECT_IS_TOP_LEVEL)
		set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/dist"
			CACHE PATH "Installation location" FORCE)
	endif ()
	enable_testing()
	add_subdirectory(demo)
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
