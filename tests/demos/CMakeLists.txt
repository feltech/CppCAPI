# Copyright 2022 David Feltell
# SPDX-License-Identifier: MIT
#------------------------------------------------------------
# Common

function(set_default_target_properties target_name)
	if (CMAKE_BUILD_TYPE STREQUAL Debug AND CMAKE_CXX_COMPILER_ID STREQUAL GNU)
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

function(set_default_standalone_host_target_properties target_name)
	set_default_target_properties(${target_name})

	target_include_directories(
		${target_name}
		PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
	)
	target_link_libraries(
		${target_name}
		PUBLIC
		cppcapi
	)
	install(
		TARGETS ${target_name}
		EXPORT ${PROJECT_NAME}_EXPORTED_TARGETS
		LIBRARY
		DESTINATION ${CMAKE_INSTALL_LIBDIR}
		RUNTIME
		DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
endfunction()

function(set_default_host_lib_target_properties target_name)
	set_default_standalone_host_target_properties(${target_name})

	generate_export_header(
		${target_name}
		BASE_NAME CPPCAPI_DEMO_HOST
		EXPORT_FILE_NAME host_export.h
	)
endfunction()

function(set_default_plugin_lib_target_properties target_name)
	set_default_target_properties(${target_name})

	set_target_properties(
		${target_name}
		PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
	)

	target_include_directories(
		${target_name}
		PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
	)
	target_link_libraries(
		${target_name}
		PRIVATE
		cppcapi
	)
	generate_export_header(
		${target_name}
		BASE_NAME CPPCAPI_DEMO_PLUGIN
		EXPORT_FILE_NAME plugin_export.h
	)
	install(
		TARGETS ${target_name}
		EXPORT ${PROJECT_NAME}_EXPORTED_TARGETS
		DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
	)
endfunction()


function(add_demo_test_target target_name)
	get_target_property(exe ${target_name} OUTPUT_NAME)

	add_custom_target(
		${target_name}.test
		COMMAND ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/${exe}
	)
	add_dependencies(${target_name}.test cppcapi.install)

	add_test(
		NAME ${target_name}.test
		COMMAND
		env CPPCAPI_PLUGIN_PATH=${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
		${CMAKE_COMMAND} --build ${PROJECT_BINARY_DIR} --target ${target_name}.test
	)
endfunction()


#------------------------------------------------------------
# Demos

add_subdirectory(hello_plugin)
add_subdirectory(string_map)
