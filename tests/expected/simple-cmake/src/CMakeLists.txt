add_library(
    simple-cmake
    simple/simple-cmake.hpp
    simple/simple-cmake.cpp
    )
add_library(simple::simple-cmake ALIAS simple-cmake)
target_include_directories(simple-cmake
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )
