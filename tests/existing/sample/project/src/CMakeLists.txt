set(sources
    # sources
    project/source1.c
    project/source2.cc
    project/subfolder/source4.cxx
    )

set(headers
    # headers
    project/header1.h
    project/subfolder/header4.hxx)

add_library(my_project_lib
    # sources
    project/source1.c
    project/source2.cc
    project/subfolder/source3.cpp
)

add_library(my_project_lib2
    # sources
    project/source1.c
    project/header1.h
  )

target_sources(my_project_lib
    PRIVATE
        # headers
        project/header2.hh)

add_library(my_weird_interface_lib INTERFACE)
target_sources(my_weird_interface_lib
    INTERFACE
        # sources
        project/subfolder/header3.hpp
        project/subfolder/source3.cpp)

# sources
# headers
