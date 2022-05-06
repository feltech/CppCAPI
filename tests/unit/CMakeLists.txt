
#------------------------------------------------------------
# Install testing libraries

# Install Catch2 testing library
CPMAddPackage("gh:catchorg/Catch2@2.13.8")
include(${Catch2_SOURCE_DIR}/contrib/Catch.cmake)

# Install Trompeloeil mocking library
CPMAddPackage("gh:rollbear/trompeloeil@42")


#------------------------------------------------------------
# Test executable target

add_executable(
	cppcapi.test
	main.cpp
	cppcapi/service/test_suite_decorator.cpp
	main.cpp
)

target_link_libraries(cppcapi.test
	PRIVATE
	project_options project_warnings
	cppcapi)

target_link_system_libraries(cppcapi.test
	PRIVATE
	Catch2 trompeloeil)

catch_discover_tests(cppcapi.test)