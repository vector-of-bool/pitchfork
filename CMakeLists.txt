cmake_minimum_required(VERSION 3.12)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(pitchfork
    VERSION 0.0.1
    LANGUAGES C CXX
    DESCRIPTION "Libraries and programs for working with Pitchfork Layout projects"
    )

if(NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
endif()

# Ensure Conan deps are up-to-date
find_package(Conan)
if(Conan_FOUND)
    conan_install(-b catch2)
endif()

# Import the deps that were setup by Conan
find_package(spdlog 1.1.0 REQUIRED)
find_package(Boost 1.68.0 REQUIRED)

# Include embedded third-party components we need for the build
add_subdirectory(third_party)

find_package(Filesystem REQUIRED)
find_package(Bikeshed)
find_package(Yarn)

if(NOT MSVC)
    add_compile_options(-Wall -Wextra)
    if(PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        add_compile_options(-Werror)
    endif()
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
