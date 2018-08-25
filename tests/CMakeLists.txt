find_package(Catch2 2.3.0 REQUIRED)
include(Catch)

add_library(pf-catch-main STATIC main.cpp)
target_link_libraries(pf-catch-main PUBLIC Catch2::Catch2)

function(pf_add_test_exe name)
    add_executable(${ARGV})
    target_link_libraries(${name} PRIVATE pf-catch-main pf::pitchfork)
    catch_discover_tests(${name})
endfunction()

pf_add_test_exe(first my_test.cpp)
