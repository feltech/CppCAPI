# Copyright 2022 David Feltell
# SPDX-License-Identifier: MIT
add_library(cppcapi INTERFACE)
add_library(${PROJECT_NAME}::cppcapi ALIAS cppcapi)
target_include_directories(
	cppcapi
	INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(
	cppcapi
	INTERFACE
	# Support module library loading during execution.
	${CMAKE_DL_LIBS}
)
install(
	DIRECTORY
	${CMAKE_CURRENT_LIST_DIR}/include/cppcapi
	DESTINATION
	${CMAKE_INSTALL_INCLUDEDIR}
)
install(
	TARGETS cppcapi
	EXPORT ${PROJECT_NAME}_EXPORTED_TARGETS
)
