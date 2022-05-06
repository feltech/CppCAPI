# Copyright 2022 David Feltell
# SPDX-License-Identifier: MIT
#------------------------------------------------------------
# C interface

install(
	DIRECTORY
	include/cppcapi-demo-string_map
	DESTINATION
	${CMAKE_INSTALL_INCLUDEDIR}
)


#------------------------------------------------------------
# Plugin module

add_library(
	cppcapi.demo.string_map.plugin
	MODULE
	plugin/client.cpp
	plugin/service.cpp
)
set_default_plugin_lib_target_properties(cppcapi.demo.string_map.plugin)
set_target_properties(
	cppcapi.demo.string_map.plugin
	PROPERTIES
	OUTPUT_NAME cppcapi-demo-string_map-plugin
)


#------------------------------------------------------------
# Host library

# Splitting host interface out of executable into a library means
# * Bindings to other languages can be made to this library (via the C interface).
# * We can use `generate_export_header` CMake convenience.

add_library(
	cppcapi.demo.string_map.host.lib
	SHARED
	host/service.cpp
	host/client.cpp
)
target_link_libraries(
	cppcapi.demo.string_map.host.lib
	PRIVATE
	stdc++
)
set_default_host_lib_target_properties(cppcapi.demo.string_map.host.lib)
set_target_properties(
	cppcapi.demo.string_map.host.lib
	PROPERTIES
	OUTPUT_NAME cppcapi-demo-string_map-host
)

#------------------------------------------------------------
# Host executable

add_executable(
	cppcapi.demo.string_map.host.app
	app/main.cpp
)
set_default_target_properties(cppcapi.demo.string_map.host.app)
set_target_properties(
	cppcapi.demo.string_map.host.app
	PROPERTIES
	OUTPUT_NAME cppcapi-demo-string_map
)
target_link_libraries(
	cppcapi.demo.string_map.host.app
	PRIVATE
	cppcapi.demo.string_map.host.lib
)
add_dependencies(
	cppcapi.demo.string_map.host.app
	cppcapi.demo.string_map.plugin
)
install(
	TARGETS cppcapi.demo.string_map.host.app
	EXPORT ${PROJECT_NAME}_EXPORTED_TARGETS
)


#------------------------------------------------------------
# CTest target

add_demo_test_target(cppcapi.demo.string_map.host.app)
