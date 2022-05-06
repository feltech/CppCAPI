# Copyright 2022 David Feltell
# SPDX-License-Identifier: MIT

#------------------------------------------------------------
# C interface

install(
	DIRECTORY
	include/cppcapi-demo-hello_plugin
	DESTINATION
	${CMAKE_INSTALL_INCLUDEDIR}
)


#------------------------------------------------------------
# Host executable

add_executable(
	cppcapi.demo.hello_plugin.host.app
	app/main.cpp
)
set_default_standalone_host_target_properties(cppcapi.demo.hello_plugin.host.app)
set_target_properties(
	cppcapi.demo.hello_plugin.host.app
	PROPERTIES
	OUTPUT_NAME cppcapi-demo-hello_plugin
)
add_dependencies(
	cppcapi.demo.hello_plugin.host.app
	cppcapi.demo.hello_plugin.plugin
)


#------------------------------------------------------------
# Plugin module

add_library(
	cppcapi.demo.hello_plugin.plugin
	MODULE
	plugin/plugin.cpp
)
set_default_plugin_lib_target_properties(cppcapi.demo.hello_plugin.plugin)
set_target_properties(
	cppcapi.demo.hello_plugin.plugin
	PROPERTIES
	OUTPUT_NAME cppcapi-demo-hello_plugin-plugin
)


#------------------------------------------------------------
# CTest target

add_demo_test_target(cppcapi.demo.hello_plugin.host.app)
