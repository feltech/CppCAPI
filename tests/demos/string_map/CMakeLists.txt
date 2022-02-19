#------------------------------------------------------------
# C interface

install(
	DIRECTORY
	include/feltpluginsystem-demo-string_map
	DESTINATION
	${CMAKE_INSTALL_INCLUDEDIR}
)


#------------------------------------------------------------
# Host executable

add_executable(
	feltpluginsystem.demo.string_map.host.app
	app/main.cpp
)
set_default_target_properties(feltpluginsystem.demo.string_map.host.app)
set_target_properties(
	feltpluginsystem.demo.string_map.host.app
	PROPERTIES
	OUTPUT_NAME feltpluginsystem-demo-string_map
)
target_link_libraries(
	feltpluginsystem.demo.string_map.host.app
	PRIVATE
	feltpluginsystem.demo.string_map.host.lib
)
add_dependencies(
	feltpluginsystem.demo.string_map.host.app
	feltpluginsystem.demo.string_map.plugin
)
install(
	TARGETS feltpluginsystem.demo.string_map.host.app
	EXPORT ${PROJECT_NAME}_EXPORTED_TARGETS
)


#------------------------------------------------------------
# Host library

# Use library rather than just executable mostly so we can use `generate_export_header`.
add_library(
	feltpluginsystem.demo.string_map.host.lib
	SHARED
	host/service.cpp
	host/client.cpp
)
set_default_host_lib_target_properties(feltpluginsystem.demo.string_map.host.lib)
set_target_properties(
	feltpluginsystem.demo.string_map.host.lib
	PROPERTIES
	OUTPUT_NAME feltpluginsystem-demo-string_map-host
)


#------------------------------------------------------------
# Plugin library

add_library(
	feltpluginsystem.demo.string_map.plugin
	MODULE
	plugin/client.cpp
	plugin/service.cpp
)
set_default_target_properties(feltpluginsystem.demo.string_map.plugin)
set_target_properties(
	feltpluginsystem.demo.string_map.plugin
	PROPERTIES
	OUTPUT_NAME feltpluginsystem-demo-string_map-plugin
	LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
)
target_include_directories(
	feltpluginsystem.demo.string_map.plugin
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)
target_link_libraries(
	feltpluginsystem.demo.string_map.plugin
	PRIVATE
	feltpluginsystem
)
generate_export_header(
	feltpluginsystem.demo.string_map.plugin
	BASE_NAME FELTPLUGINSYSTEM_DEMO_PLUGIN
	EXPORT_FILE_NAME plugin_export.h
)
install(
	TARGETS feltpluginsystem.demo.string_map.plugin
	EXPORT ${PROJECT_NAME}_EXPORTED_TARGETS
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
)


#------------------------------------------------------------
# CTest target

add_demo_test_target(feltpluginsystem.demo.string_map.host.app)
