find_package(Catch2 2.3.0 REQUIRED)

add_executable(first-test main.cpp my_test.cpp)
target_link_libraries(first-test PRIVATE pf::pitchfork Catch2::Catch2)
add_test(NAME first-test COMMAND first-test)
