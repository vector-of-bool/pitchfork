add_library(
  libpitchfork
  pf/fs.hpp
  pf/pitchfork.hpp
  pf/pitchfork.cpp
  pf/new.hpp
  pf/new.cpp
  pf/new/params.hpp
  pf/new/dirs.hpp
  pf/new/dirs.cpp
  )
set_property(TARGET libpitchfork PROPERTY OUTPUT_NAME pf)
add_library(pf::pitchfork ALIAS libpitchfork)
target_include_directories(
  libpitchfork
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  )
target_link_libraries(libpitchfork PUBLIC CXX::Filesystem Boost::boost)

add_executable(pf pf.cpp)
target_link_libraries(pf PRIVATE
    pf::pitchfork
    taywee::args
    spdlog::spdlog
    )
