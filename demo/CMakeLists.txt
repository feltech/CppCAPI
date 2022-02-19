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
	${CMAKE_CURRENT_LIST_DIR}/include/feltpluginsystemdemo
	DESTINATION
	${CMAKE_INSTALL_INCLUDEDIR}
)


#------------------------------------------------------------
# Host library

# Use library rather than just executable mostly so we can use `generate_export_header`.
add_library(
	feltpluginsystem.demo.host.lib
	SHARED
	host/service.cpp
	host/client.cpp
)
set_default_target_properties(feltpluginsystem.demo.host.lib)
set_target_properties(
	feltpluginsystem.demo.host.lib
	PROPERTIES
	OUTPUT_NAME feltpluginsystemdemohost
)
target_include_directories(
	feltpluginsystem.demo.host.lib
	PUBLIC
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/demo/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)
target_link_libraries(
	feltpluginsystem.demo.host.lib
	PUBLIC
	feltpluginsystem
)
set(plugin_path ${CMAKE_INSTALL_PREFIX}/)
target_compile_definitions(
	feltpluginsystem.demo.host.lib
	PUBLIC
	$<BUILD_INTERFACE:FELTPLUGINSYSTEM_PLUGIN_PATH=\"${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}\">
	$<INSTALL_INTERFACE:FELTPLUGINSYSTEM_PLUGIN_PATH=\"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}\">
)
generate_export_header(
	feltpluginsystem.demo.host.lib
	EXPORT_FILE_NAME feltpluginsystemdemohost_export.h
)
install(
	TARGETS feltpluginsystem.demo.host.lib
	EXPORT ${PROJECT_NAME}_EXPORTED_TARGETS
)


#------------------------------------------------------------
# Host executable

add_executable(
	feltpluginsystem.demo.host
	app/main.cpp
)
set_default_target_properties(feltpluginsystem.demo.host)
set_target_properties(
	feltpluginsystem.demo.host
	PROPERTIES
	OUTPUT_NAME feltpluginsystemdemo
)
target_link_libraries(
	feltpluginsystem.demo.host
	PRIVATE
	feltpluginsystem.demo.host.lib
)
add_dependencies(
	feltpluginsystem.demo.host
	feltpluginsystem.demo.plugin
)
install(
	TARGETS feltpluginsystem.demo.host
	EXPORT ${PROJECT_NAME}_EXPORTED_TARGETS
)


#------------------------------------------------------------
# Plugin library

add_library(
	feltpluginsystem.demo.plugin
	MODULE
	plugin/client.cpp
	plugin/service.cpp
)
set_default_target_properties(feltpluginsystem.demo.plugin)
set_target_properties(
	feltpluginsystem.demo.plugin
	PROPERTIES
	OUTPUT_NAME feltpluginsystemdemoplugin
	LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
)
target_include_directories(
	feltpluginsystem.demo.plugin
	PUBLIC
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/demo/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)
target_link_libraries(
	feltpluginsystem.demo.plugin
	PRIVATE
	feltpluginsystem
)
generate_export_header(
	feltpluginsystem.demo.plugin
	EXPORT_FILE_NAME feltpluginsystemdemoplugin_export.h
)
install(
	TARGETS feltpluginsystem.demo.plugin
	EXPORT ${PROJECT_NAME}_EXPORTED_TARGETS
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
)


#------------------------------------------------------------
# CTest target

add_custom_target(
	feltpluginsystem.demo.install
	COMMAND ${CMAKE_COMMAND} --install ${PROJECT_BINARY_DIR}
)

add_custom_target(
	feltpluginsystem.demo.test
	COMMAND ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/feltpluginsystemdemo
)
add_dependencies(feltpluginsystem.demo.test feltpluginsystem.demo.install)

add_test(
	NAME FeltPluginDemo
	COMMAND ${CMAKE_COMMAND} --build ${PROJECT_BINARY_DIR} --target feltpluginsystem.demo.test)

