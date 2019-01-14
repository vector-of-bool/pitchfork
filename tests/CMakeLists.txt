find_package(Catch2 2.3.0 REQUIRED)
include(Catch)

set(PF_TEST_BINDIR "${CMAKE_CURRENT_BINARY_DIR}")
set(PF_TEST_SRCDIR "${CMAKE_CURRENT_SOURCE_DIR}")

add_library(pf-test-lib STATIC test-main.cpp compare_fs.hpp compare_fs.cpp)
target_link_libraries(pf-test-lib PUBLIC Catch2::Catch2 pf::pitchfork)
target_compile_definitions(pf-test-lib
    PUBLIC
        "PF_TEST_BINDIR=\"${CMAKE_CURRENT_BINARY_DIR}\""
        "PF_TEST_SRCDIR=\"${CMAKE_CURRENT_SOURCE_DIR}\""
    )

function(configure_directory directory)
    file(GLOB_RECURSE directory_contents CONFIGURE_DEPENDS "${directory}/*")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${directory_contents})

    file(COPY "${directory}/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/${directory}/")
    file(GLOB_RECURSE directory_templates RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${directory}/*.in")

    foreach(file ${directory_templates})
        string(LENGTH "${file}" len)
        math(EXPR without_dot_in "${len} - 3")
        string(SUBSTRING "${file}" 0 ${without_dot_in} template_out)
        configure_file("${file}" "${CMAKE_CURRENT_BINARY_DIR}/${template_out}")
    endforeach()
endfunction()

function(pf_add_test_exe name)
    add_executable(${ARGV})
    target_link_libraries(${name} PRIVATE pf-test-lib)
    catch_discover_tests(${name}
        TEST_PREFIX ${name}::
        EXTRA_ARGS $<$<BOOL:${PF_FORCE_TESTS_COLOR}>:--use-colour=yes>)
endfunction()

function(pf_add_query_test ID)
    set(options)
    set(list_args PASS_REGULAR_EXPRESSION FAIL_REGULAR_EXPRESSION)

    cmake_parse_arguments(PARSE_ARGV 1 ARG "${options}" "${args}" "${list_args}")

    add_test(
        NAME "query:${ID}"
        COMMAND pf query "${ID}"
    )
    set_tests_properties("query:${ID}"
        PROPERTIES
            PASS_REGULAR_EXPRESSION "${ARG_PASS_REGULAR_EXPRESSION}"
            FAIL_REGULAR_EXPRESSION "${ARG_FAIL_REGULAR_EXPRESSION}"
    )
endfunction()

pf_add_test_exe(generate generate.cpp)

pf_add_test_exe(existing
    existing/detect_base_dir.cpp
    existing/update_source_files.cpp)
configure_directory(existing/sample)

pf_add_query_test(project.root
    PASS_REGULAR_EXPRESSION "${PROJECT_SOURCE_DIR}"
)
