include_guard(GLOBAL)

include(CMakePackageConfigHelpers)

# Real impl of `pf_auto()`
function(_pf_auto)
    set(options
        NO_INSTALL
        )
    set(args
        LIBRARY_NAME
        ALIAS
        OUTPUT_NAME
        VERSION_COMPATIBILITY
        )
    set(list_args
        LINK
        PRIVATE_LINK
        EXE_LINK
        SUBDIR_BEFORE
        SUBDIR_AFTER
        )
    cmake_parse_arguments(PARSE_ARGV 0 ARG "${options}" "${args}" "${list_args}")

    foreach(arg IN LISTS ARG_UNPARSED_ARGUMENTS)
        message(WARNING "Unknown argument to pf_auto: ${arg}")
    endforeach()

    # Default arguments
    if(NOT DEFINED ARG_LIBRARY_NAME)
        set(ARG_LIBRARY_NAME "${PROJECT_NAME}")
    endif()
    if(NOT DEFINED ARG_ALIAS)
        set(ARG_ALIAS "${PROJECT_NAME}::${ARG_LIBRARY_NAME}")
    endif()
    if(NOT DEFINED ARG_VERSION_COMPATIBILITY)
        set(ARG_VERSION_COMPATIBILITY ExactVersion)
    endif()

    # Check arguments
    if(NOT ARG_ALIAS MATCHES "::")
        message(SEND_ERROR "pf_auto(ALIAS) must contain a double-colon '::'")
    endif()

    # Check that we were call correctly
    # Must have called project():
    if(NOT DEFINED PROJECT_NAME)
        message(FATAL_ERROR "Please call project() before pf_auto()")
    endif()

    # Before defining our project, add the subdirs
    foreach(subdir IN LISTS ARG_SUBDIR_BEFORE)
        add_subdirectory("${subdir}")
    endforeach()

    # Must be called in the same dir as a project():
    get_filename_component(pr_dir "${PROJECT_SOURCE_DIR}" ABSOLUTE)
    get_filename_component(here_dir "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
    if(NOT pr_dir STREQUAL here_dir)
        message(FATAL_ERROR "pf_auto() must be called in the same directory as the matching project() call.")
    endif()

    # Some basic vars:
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

    # Bool-ish for checking if we are the root project
    get_directory_property(pr_pardir "${PROJECT_SOURCE_DIR}" DIRECTORY)
    set(is_root_project TRUE)
    if(pr_pardir)
        set(is_root_project FALSE)
    endif()

    # Should we install?
    set(do_install FALSE)
    if(is_root_project AND NOT ARG_NO_INSTALL)
        set(do_install TRUE)
    endif()
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(arch x64)
    else()
        set(arch x86)
    endif()
    set(install_infix "lib/${PROJECT_NAME}-${PROJECT_VERSION}-${arch}")
    set(install_target_common
        EXPORT "${PROJECT_NAME}Targets"
        RUNTIME DESTINATION "${install_infix}/bin"
        LIBRARY DESTINATION "${install_infix}/lib"
        ARCHIVE DESTINATION "${install_infix}/lib"
        OBJECTS DESTINATION "${install_infix}/lib"
        INCLUDES DESTINATION "${install_infix}/include"
        )

    # Calculate the public include directory
    set(pub_inc_dir "${PROJECT_SOURCE_DIR}/include")
    if(NOT IS_DIRECTORY "${pub_inc_dir}")
        set(pub_inc_dir "${PROJECT_SOURCE_DIR}/src")
    endif()

    # Private inc dir is always the same:
    set(priv_inc_dir "${PROJECT_SOURCE_DIR}/src")

    # Get our source files
    file(GLOB_RECURSE
        sources
        RELATIVE "${PROJECT_SOURCE_DIR}/src"
        CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/src/*"
        )
    # Maintain three different source classifications
    set(exe_sources)
    set(lib_sources)
    set(test_sources)
    # Find them
    foreach(file IN LISTS sources)
        get_filename_component(fname "${file}" NAME)
        if(fname STREQUAL file)
            # File is not in subdirectory. It is an executable
            list(APPEND exe_sources "src/${file}")
        elseif(fname MATCHES "\\.test\\.[cC][a-zA-Z]+^")
            list(APPEND test_sources "src/${file}")
        else()
            list(APPEND lib_sources "src/${file}")
        endif()
    endforeach()
    # Add all headers to the library sources
    if(NOT pub_inc_dir STREQUAL priv_inc_dir)
        file(GLOB_RECURSE includes "${pub_inc_dir}/*")
        list(APPEND lib_sources ${includes})
    endif()

    # Pull in external before we check for link targets
    get_filename_component(external_dir "${PROJECT_SOURCE_DIR}/external" ABSOLUTE)
    if(EXISTS "${external_dir}/CMakeLists.txt")
        add_subdirectory("${external_dir}")
    endif()

    foreach(link IN LISTS ARG_LINK ARG_PRIVATE_LINK ARG_EXE_LINK)
        if(NOT TARGET "${link}")
            message(SEND_ERROR "Requested linking to non-target: ${link}")
        endif()
    endforeach()

    # Define the library
    add_library("${ARG_LIBRARY_NAME}" ${lib_sources})
    target_include_directories("${ARG_LIBRARY_NAME}" PUBLIC "$<BUILD_INTERFACE:${pub_inc_dir}>")
    if(NOT pub_inc_dir STREQUAL priv_inc_dir)
        target_include_directories("${ARG_LIBRARY_NAME}" PRIVATE "${priv_inc_dir}")
    endif()
    # Link dependencies
    target_link_libraries("${ARG_LIBRARY_NAME}" PUBLIC ${ARG_LINK})
    foreach(priv IN LISTS ARG_PRIVATE_LINK)
        target_link_libraries("${ARG_LIBRARY_NAME}" PRIVATE $<BUILD_INTERFACE:${priv}>)
    endforeach()
    # Add an alias target
    add_library("${ARG_ALIAS}" ALIAS "${ARG_LIBRARY_NAME}")
    set_property(TARGET "${ARG_LIBRARY_NAME}" PROPERTY EXPORT_NAME "${ARG_ALIAS}")
    # Optional properies
    if(DEFINED ARG_OUTPUT_NAME)
        set_property(TARGET "${ARG_LIBRARY_NAME}" PROPERTY OUTPUT_NAME "${ARG_OUTPUT_NAME}")
    endif()

    if(do_install)
        install(TARGETS "${ARG_LIBRARY_NAME}" ${install_target_common})
        install(
            DIRECTORY "${pub_inc_dir}"
            DESTINATION "${install_infix}/include"
            FILES_MATCHING
                PATTERN *.h
                PATTERN *.hpp
                PATTERN *.hh
                PATTERN *.h++
                PATTERN *.hxx
                PATTERN *.H
            )
    endif()

    # Add executables
    foreach(exe_source IN LISTS exe_sources)
        get_filename_component(exe_name "${exe_source}" NAME_WE)
        add_executable("${exe_name}" "${exe_source}")
        target_link_libraries("${exe_name}" PRIVATE "${ARG_ALIAS}" ${ARG_EXE_LINK})
        if(do_install)
            install(TARGETS "${exe_name}" RUNTIME DESTINATION bin)
        endif()
    endforeach()

    get_directory_property(already_subdirs SUBDIRECTORIES)

    # Add the tests subdirectory
    get_filename_component(tests_dir "${PROJECT_SOURCE_DIR}/tests" ABSOLUTE)
    option(BUILD_TESTING "Build the testing tree" ON)
    set(_PF_ADDED_TESTS FALSE PARENT_SCOPE)
    if(EXISTS "${tests_dir}/CMakeLists.txt" AND BUILD_TESTING AND is_root_project AND NOT tests_dir IN_LIST already_subdirs)
        add_subdirectory("${tests_dir}")
        set(_PF_ADDED_TESTS TRUE PARENT_SCOPE)
    endif()

    # Add the examples subdirectory
    get_filename_component(examples_dir "${PROJECT_SOURCE_DIR}/examples" ABSOLUTE)
    option(BUILD_EXAMPLES "Build examples" ON)
    if(EXISTS "${examples_dir}/CMakeLists.txt" AND BUILD_EXAMPLES AND is_root_project AND NOT examples_dir IN_LIST already_subdirs)
        add_subdirectory("${examples_dir}")
    endif()

    # Add the extras subdirectory
    get_filename_component(extras_dir "${PROJECT_SOURCE_DIR}/extras" ABSOLUTE)
    if(EXISTS "${extras_dir}/CMakeLists.txt" AND NOT extras_dirs IN_LIST already_subdirs)
        add_subdirectory("${extras_dir}")
    endif()

    # Add the data subdirectory
    get_filename_component(data_dir "${PROJECT_SOURCE_DIR}/data" ABSOLUTE)
    if(EXISTS "${data_dir}/CMakeLists.txt" AND NOT data_dir IN_LIST already_subdirs)
        add_subdirectory("${data_dir}")
    endif()

    if(do_install)
        install(
            EXPORT "${PROJECT_NAME}Targets"
            DESTINATION "${install_infix}/cmake"
            FILE "${PROJECT_NAME}Config.cmake"
            )
        write_basic_package_version_file(
            "${PROJECT_NAME}ConfigVersion.cmake"
            COMPATIBILITY ${ARG_VERSION_COMPATIBILITY}
            )
        install(
            FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
            DESTINATION "${install_infix}/cmake"
            )
    endif()
endfunction()

# Macro so we can do things in the parent scope
macro(pf_auto)
    _pf_auto(${ARGN})
    # Maybe call enable_testing() in the caller's scope (important)
    if(_PF_ADDED_TESTS)
        enable_testing()
    endif()
endmacro()
