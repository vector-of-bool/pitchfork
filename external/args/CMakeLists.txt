if(NOT TARGET taywee::args)
    add_library(taywee::args IMPORTED INTERFACE GLOBAL)
    target_include_directories(taywee::args INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")
endif()