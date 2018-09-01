#include "./cmake.hpp"

#include <pf/new/project.hpp>

#include <kainjow/mustache.hpp>

#include <cmrc/cmrc.hpp>

CMRC_DECLARE(pf_templates);

void pf::create_cmake_files(const pf::new_project_params& params, std::error_code& ec) {
    kainjow::mustache::data ctx;
    const auto              alias_target = params.root_namespace + "::" + params.name;
    ctx.set("alias_target", alias_target);
    ctx.set("root_ns", params.root_namespace);
    ctx.set("project_name", params.name);
    ctx.set("ns_path", path_for_namespace(params.root_namespace).string());
    ctx.set("gen_extras", params.create_extras);
    ctx.set("gen_examples", params.create_examples);
    ctx.set("gen_third_party", params.create_third_party);
    ctx.set("gen_tetss", params.create_tests);
    ctx.set("separate_headers", params.separate_headers);

    auto templates_fs = cmrc::pf_templates::get_filesystem();

    auto render = [&](std::string path) {
        auto resource = templates_fs.open(path);
        auto mustache = kainjow::mustache::mustache{std::string{resource.begin(), resource.end()}};
        if (!mustache.is_valid()) {
            throw std::runtime_error("Error rendering template file: " + path + ": "
                                     + mustache.error_message());
        }
        return mustache.render(ctx);
    };

    write_file(params.directory / "src/CMakeLists.txt", render("cmake/src_cml.in.cmake"), ec);
    if (ec) {
        return;
    }

    write_file(params.directory / "CMakeLists.txt", render("cmake/root_cml.in.cmake"), ec);
    if (ec) {
        return;
    }

    // Optional content
    if (params.create_examples) {
        write_file(params.directory / "examples/CMakeLists.txt",
                   render("cmake/examples_cml.in.cmake"),
                   ec);
        if (ec) {
            return;
        }
    }
}
