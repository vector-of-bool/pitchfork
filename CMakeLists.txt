cmake_minimum_required(VERSION 3.12)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(pitchfork VERSION 0.0.1 DESCRIPTION "Libraries and programs for working with Pitchfork Layout projects")

# Include third-party components we need for the build
add_subdirectory(third_party)

find_package(Filesystem REQUIRED)
find_package(Bikeshed)
find_package(Yarn)

if(NOT Bikeshed_FOUND)
    message(STATUS "No Bikeshed executable, so we will not generate the spec document")
endif()

add_subdirectory(src)
add_subdirectory(data)

option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS AND (PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
    enable_testing()
    add_subdirectory(tests)
endif()

option(BUILD_EXAMPLES "Build examples" ON)
if(BUILD_EXAMPLES AND (PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
    add_subdirectory(examples)
endif()

add_subdirectory(extras)
