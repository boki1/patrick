
cmake_minimum_required(VERSION 3.22)
project(cpp-template)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib)

option(CPP_TEMPLATE_BUILD_TESTS "Whether to build tests." "ON")
if("${CPP_TEMPLATE_BUILD_TESTS}" STREQUAL "ON")
	enable_testing()
	include(cmake/conan.cmake)
    add_subdirectory(tests)
endif()
