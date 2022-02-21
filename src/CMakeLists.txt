# Copyright 2022 David Feltell
# SPDX-License-Identifier: MIT
add_library(feltpluginsystem INTERFACE)
target_include_directories(
	feltpluginsystem
	INTERFACE
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/include>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(
	feltpluginsystem
	INTERFACE
	# Support module library loading during execution.
	${CMAKE_DL_LIBS}
)
install(
	DIRECTORY
	${CMAKE_CURRENT_LIST_DIR}/include/feltpluginsystem
	DESTINATION
	${CMAKE_INSTALL_INCLUDEDIR}
)
install(
	TARGETS feltpluginsystem
	EXPORT ${PROJECT_NAME}_EXPORTED_TARGETS
)