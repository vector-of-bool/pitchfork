add_executable(example1 example1.cpp)

target_link_libraries(example1 PRIVATE {{alias_target}})
