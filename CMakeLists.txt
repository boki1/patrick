cmake_minimum_required(VERSION 3.22)
project(patrick)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use conan for dependecies of all targets.
include(cmake/conan.cmake)

add_subdirectory(patrick)

option(PATRICK_BUILD_TESTS "Whether to build tests." "ON")
if("${PATRICK_BUILD_TESTS}" STREQUAL "ON")
  enable_testing()
  add_subdirectory(tests)
endif()

option(PATRICK_BUILD_LIVEDEMO "Whether to build live demo." "ON")
if("${PATRICK_BUILD_LIVEDEMO}" STREQUAL "ON")
  add_subdirectory(livedemo)
endif()
