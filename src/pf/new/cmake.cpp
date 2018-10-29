#include "./cmake.hpp"

#include <pf/file_template.hpp>
#include <pf/new/project.hpp>

#include <spdlog/fmt/ostr.h>

#include <kainjow/mustache.hpp>

#include <cmrc/cmrc.hpp>

CMRC_DECLARE(pf_templates);

void pf::create_cmake_files(const pf::new_project_params& params) {
    pf::template_renderer trr{params.directory, cmrc::pf_templates::get_filesystem()};
    // Fill out the template data:
    const auto alias_target = params.root_namespace + "::" + params.name;
    trr.set("alias_target", alias_target);
    trr.set("root_ns", params.root_namespace);
    trr.set("project_name", params.name);
    trr.set("ns_path", path_for_namespace(params.root_namespace).string());
    trr.set("gen_extras", params.create_extras);
    trr.set("gen_examples", params.create_examples);
    trr.set("gen_third_party", params.create_third_party);
    trr.set("gen_tests", params.create_tests);
    trr.set("separate_headers", params.separate_headers);
    trr.set("first_stem", params.first_file_stem);

    trr.render_to_file("cmake/src_cml.in.cmake", "src/CMakeLists.txt");
    trr.render_to_file("cmake/root_cml.in.cmake", "CMakeLists.txt");

    // Optional content
    if (params.create_examples) {
        trr.render_to_file("cmake/examples_cml.in.cmake", "examples/CMakeLists.txt");
    }
}
