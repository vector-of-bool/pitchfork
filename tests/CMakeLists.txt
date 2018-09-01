find_package(Catch2 2.3.0 REQUIRED)
include(Catch)

add_library(pf-test-lib STATIC test-main.cpp compare_fs.hpp compare_fs.cpp)
target_link_libraries(pf-test-lib PUBLIC Catch2::Catch2 pf::pitchfork)
target_compile_definitions(pf-test-lib
    PUBLIC
        "PF_TEST_BINDIR=\"${CMAKE_CURRENT_BINARY_DIR}\""
        "PF_TEST_SRCDIR=\"${CMAKE_CURRENT_SOURCE_DIR}\""
    )

function(pf_add_test_exe name)
    add_executable(${ARGV})
    target_link_libraries(${name} PRIVATE pf-test-lib)
    catch_discover_tests(${name})
endfunction()

pf_add_test_exe(first my_test.cpp)
