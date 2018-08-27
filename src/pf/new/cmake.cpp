#include "./cmake.hpp"

#include <pf/new/project.hpp>

#include <kainjow/mustache.hpp>

namespace {
auto ROOT_CML = R"CML(cmake_minimum_required(VERSION 3.10)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}")

project({{project_name}} VERSION 0.0.1 DESCRIPTION "A great new project")

{{ #gen_third_party %}}
# Include third-party components we need for the build
add_subdirectory(third_party)
{{% /gen_third_party }}

add_subdirectory(src)

option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS AND (PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
    enable_testing()
    add_subdirectory(tests)
endif()

{{ #gen_examples %}}
option(BUILD_EXAMPLES "Build examples" ON)
if(BUILD_EXAMPLES AND (PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
    add_subdirectory(examples)
endif()
{{% /gen_examples }}

{{ #gen_extras %}}
add_subdirectory(extras)
{{% /gen_extras }}

)CML";

const auto SRC_CML = R"(add_library(
    {{project_name}}
    {{src_dir}}/{{first_stem}}.hpp
    {{src_dir}}/{{first_stem}}.cpp
    )
add_library({{alias_target}} ALIAS {{project_name}})
target_include_directories({{project_name}}
    {{ #separate_headers %}}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    {{% /separate_headers %}}
    {{% ^separate_headers %}}
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    {{% /separate_headers }}
    )
)";

const auto FIRST_TEST_CPP = R"(#include <iostream>

#include <{{src_dir}}/{{first_stem}}.hpp>

int main() {
    const auto value = {{root_ns}}::calculate_value();
    if (value == 42) {
        std::cout << "We calculated the value correctly\n";
        return 0;
    } else {
        std::cout << "The value was incorrect!\n";
        return 1;
    }
}
)";

auto EXAMPLES_CML = R"(add_executable(example1 example1.cpp)

target_link_libraries(example1 PRIVATE {{alias_target}})
)";

std::string render(const std::string& tmpl, const kainjow::mustache::data& ctx) {
    kainjow::mustache::mustache mstc{tmpl};
    if (!mstc.is_valid()) {
        throw std::runtime_error{"Invalid template: " + mstc.error_message()};
    }
    return mstc.render(ctx);
}

}  // namespace

void pf::create_cmake_files(const pf::new_project_params& params, std::error_code& ec) {
    kainjow::mustache::data ctx;
    const auto              alias_target = params.root_namespace + "::" + params.name;
    ctx.set("alias_target", alias_target);
    ctx.set("root_ns", params.root_namespace);
    ctx.set("project_name", params.name);
    ctx.set("src_dir", path_for_namespace(params.root_namespace).string());
    ctx.set("gen_extras", params.create_extras);
    ctx.set("gen_examples", params.create_examples);
    ctx.set("gen_third_party", params.create_third_party);
    ctx.set("separate_headers", params.separate_headers);

    write_file(params.directory / "src/CMakeLists.txt", render(SRC_CML, ctx), ec);
    if (ec) {
        return;
    }

    write_file(params.directory / "tests/my_test.cpp", render(FIRST_TEST_CPP, ctx), ec);
    if (ec) {
        return;
    }

    write_file(params.directory / "CMakeLists.txt", render(ROOT_CML, ctx), ec);
    if (ec) {
        return;
    }

    // Optional content
    if (params.create_examples) {
        write_file(params.directory / "examples/CMakeLists.txt", render(EXAMPLES_CML, ctx), ec);
        if (ec) {
            return;
        }
    }
}
