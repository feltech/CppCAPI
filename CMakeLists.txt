cmake_minimum_required(VERSION 3.21)
project(FeltPlugin)

set(CMAKE_CXX_STANDARD 17)
include(GenerateExportHeader)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
	add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
	add_link_options(-fsanitize=address -fno-omit-frame-pointer)
endif ()


#------------------------------------------------------------
# Plugin system

add_library(FeltPluginSystem INTERFACE)
target_include_directories(
	FeltPluginSystem
	INTERFACE
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/include>
)

#------------------------------------------------------------
# Host library

#Llibrary mostly so we can use `generate_export_header`.
add_library(
	FeltPluginDemoHostLib
	SHARED
	demo/host/service.cpp
	demo/host/client.cpp
)
set_target_properties(
	FeltPluginDemoHostLib
	PROPERTIES

	C_VISIBILITY_PRESET hidden
	CXX_VISIBILITY_PRESET hidden
	VISIBILITY_INLINES_HIDDEN ON
)
target_include_directories(
	FeltPluginDemoHostLib
	PUBLIC
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/demo/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)
target_link_libraries(
	FeltPluginDemoHostLib
	PUBLIC
	FeltPluginSystem
	${CMAKE_DL_LIBS}
)
generate_export_header(FeltPluginDemoHostLib)

#------------------------------------------------------------
# Host executable

add_executable(FeltPluginDemoHost demo/app/main.cpp)
target_link_libraries(FeltPluginDemoHost PRIVATE FeltPluginDemoHostLib)
set_target_properties(
	FeltPluginDemoHost
	PROPERTIES

	ENABLE_EXPORTS ON

	C_VISIBILITY_PRESET hidden
	CXX_VISIBILITY_PRESET hidden
	VISIBILITY_INLINES_HIDDEN ON
)

#------------------------------------------------------------
# Demo plugin

add_library(
	FeltPluginDemoPlugin
	MODULE
	demo/plugin/client.cpp
	demo/plugin/service.cpp
)
target_include_directories(
	FeltPluginDemoPlugin
	PUBLIC
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/demo/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)
target_link_libraries(
	FeltPluginDemoPlugin
	PRIVATE
	FeltPluginSystem
)
generate_export_header(FeltPluginDemoPlugin)
set_target_properties(
	FeltPluginDemoPlugin
	PROPERTIES
	C_VISIBILITY_PRESET hidden
	CXX_VISIBILITY_PRESET hidden
	VISIBILITY_INLINES_HIDDEN ON
)