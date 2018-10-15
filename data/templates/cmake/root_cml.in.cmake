cmake_minimum_required(VERSION 3.10)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}")

project({{project_name}} VERSION 0.0.1 DESCRIPTION "A great new project")

{{ #gen_third_party %}}
# Include third-party components we need for the build
add_subdirectory(third_party)
{{% /gen_third_party }}

add_subdirectory(src)

{{ #gen_tests %}}
option(BUILD_TESTING "Build tests" ON)
if(BUILD_TESTING AND (PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
    enable_testing()
    add_subdirectory(tests)
endif()
{{% /gen_tests }}

{{ #gen_examples %}}
option(BUILD_EXAMPLES "Build examples" ON)
if(BUILD_EXAMPLES AND (PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
    add_subdirectory(examples)
endif()
{{% /gen_examples }}

{{ #gen_extras %}}
add_subdirectory(extras)
{{% /gen_extras }}
