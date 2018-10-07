cmake_minimum_required(VERSION 3.12)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(pitchfork
    VERSION 0.0.1
    LANGUAGES C CXX
    DESCRIPTION "Libraries and programs for working with Pitchfork Layout projects"
    )

option(PF_FORCE_TESTS_COLOR "Force colored output in tests" OFF)

if(NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
endif()

# Ensure Conan deps are up-to-date
find_package(Conan)
if(Conan_FOUND)
    conan_install()
endif()

# Import the deps that were setup by Conan
find_package(spdlog 1.1.0 REQUIRED)
find_package(Boost 1.68.0 REQUIRED)

# Other deps
find_package(Filesystem REQUIRED)
find_package(Bikeshed)
find_package(Yarn)
include(CMakeRC)

if(NOT MSVC)
    add_compile_options(-Wall -Wextra)
    if(PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        add_compile_options(-Werror)
    endif()
endif()

add_subdirectory(data)

set(PF_URL "file://${PROJECT_SOURCE_DIR}/extras/pf-cmake" CACHE INTERNAL "")
include(Pitchfork)

enable_testing()

pf_auto(
    OUTPUT_NAME pf
    ALIAS pf::pitchfork
    LINK
        CXX::Filesystem
        Boost::boost
        spdlog::spdlog
    PRIVATE_LINK
        kainjow::mustache
        pf::templates
    EXE_LINK
        taywee::args
    )

include(CPack)
