add_executable(first-test my_test.cpp)
target_link_libraries(first-test PRIVATE pf::pitchfork catch2::main)
add_test(NAME first-test COMMAND first-test)
