#------------------------------------------------------------
# Common

function(set_default_target_properties target_name)
	if (CMAKE_BUILD_TYPE STREQUAL Debug AND CMAKE_COMPILER_ID STREQUAL GNU)
		# Enable sanitizers for tests on GCC.
		target_compile_options(
			${target_name} PRIVATE -fsanitize=address -fno-omit-frame-pointer)
		target_link_options(
			${target_name} PRIVATE -fsanitize=address -fno-omit-frame-pointer)
	endif ()

	# Runtime library search path
	if (APPLE)
		set(rpath "@loader_path/../${CMAKE_INSTALL_LIBDIR}")
	else ()
		set(rpath "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
	endif ()

	set_target_properties(
		${target_name}
		# Disable all symbol visibility by default.
		PROPERTIES
		C_VISIBILITY_PRESET hidden
		CXX_VISIBILITY_PRESET hidden
		VISIBILITY_INLINES_HIDDEN ON
		# Standardise C++ version as much as possible.
		CXX_STANDARD 17
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF
		# Set build tree to roughly match install tree.
		LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}
		ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}
		RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}
		# Runtime library search path
		INSTALL_RPATH ${rpath}
	)
endfunction()


#------------------------------------------------------------
# C interface

install(
	DIRECTORY
	include/feltpluginsystem-demo-string_map
	DESTINATION
	${CMAKE_INSTALL_INCLUDEDIR}
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
set_default_target_properties(feltpluginsystem.demo.string_map.host.lib)
set_target_properties(
	feltpluginsystem.demo.string_map.host.lib
	PROPERTIES
	OUTPUT_NAME feltpluginsystem-demo-string_map-host
)
target_include_directories(
	feltpluginsystem.demo.string_map.host.lib
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)
target_link_libraries(
	feltpluginsystem.demo.string_map.host.lib
	PUBLIC
	feltpluginsystem
)
set(plugin_path ${CMAKE_INSTALL_PREFIX}/)
target_compile_definitions(
	feltpluginsystem.demo.string_map.host.lib
	PUBLIC
	$<BUILD_INTERFACE:FELTPLUGINSYSTEM_PLUGIN_PATH=\"${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}\">
	$<INSTALL_INTERFACE:FELTPLUGINSYSTEM_PLUGIN_PATH=\"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}\">
)
generate_export_header(
	feltpluginsystem.demo.string_map.host.lib
	BASE_NAME FELTPLUGINSYSTEM_DEMO_HOST
	EXPORT_FILE_NAME host_export.h
)
install(
	TARGETS feltpluginsystem.demo.string_map.host.lib
	EXPORT ${PROJECT_NAME}_EXPORTED_TARGETS
)


#------------------------------------------------------------
# Host executable

add_executable(
	feltpluginsystem.demo.string_map.host
	app/main.cpp
)
set_default_target_properties(feltpluginsystem.demo.string_map.host)
set_target_properties(
	feltpluginsystem.demo.string_map.host
	PROPERTIES
	OUTPUT_NAME feltpluginsystem-demo-string_map
)
target_link_libraries(
	feltpluginsystem.demo.string_map.host
	PRIVATE
	feltpluginsystem.demo.string_map.host.lib
)
add_dependencies(
	feltpluginsystem.demo.string_map.host
	feltpluginsystem.demo.string_map.plugin
)
install(
	TARGETS feltpluginsystem.demo.string_map.host
	EXPORT ${PROJECT_NAME}_EXPORTED_TARGETS
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

add_custom_target(
	feltpluginsystem.demo.string_map.install
	COMMAND ${CMAKE_COMMAND} --install ${PROJECT_BINARY_DIR}
)

add_custom_target(
	feltpluginsystem.demo.string_map.test
	COMMAND ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/feltpluginsystem-demo-string_map
)
add_dependencies(feltpluginsystem.demo.string_map.test feltpluginsystem.demo.string_map.install)

add_test(
	NAME FeltPluginDemo
	COMMAND ${CMAKE_COMMAND} --build ${PROJECT_BINARY_DIR} --target feltpluginsystem.demo.string_map.test)

