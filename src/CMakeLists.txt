add_library(
    libpitchfork
    pf/fs.hpp
    pf/fs.cpp
    pf/pitchfork.hpp
    pf/pitchfork.cpp
    pf/new.hpp
    pf/new.cpp
    pf/new/params.hpp
    pf/new/files.hpp
    pf/new/files.cpp
    pf/new/dirs.hpp
    pf/new/dirs.cpp
    pf/new/cmake.hpp
    pf/new/cmake.cpp
    pf/new/project.hpp
    pf/new/project.cpp
    )
set_property(TARGET libpitchfork PROPERTY OUTPUT_NAME pf)
add_library(pf::pitchfork ALIAS libpitchfork)
target_include_directories(
    libpitchfork
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )
target_link_libraries(libpitchfork
    PUBLIC
        CXX::Filesystem
        Boost::boost
        spdlog::spdlog
    PRIVATE
        kainjow::mustache
        pf::templates
    )

add_executable(pf pf.cpp)
target_link_libraries(pf PRIVATE
    pf::pitchfork
    taywee::args
    )
