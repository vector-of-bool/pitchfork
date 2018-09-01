#include "./files.hpp"

#include <pf/new/dirs.hpp>
#include <pf/new/project.hpp>

#include <cmrc/cmrc.hpp>

#include <boost/algorithm/string.hpp>
#include <kainjow/mustache.hpp>

#include <fstream>

CMRC_DECLARE(pf_templates);

void pf::create_files(const pf::new_project_params& params, std::error_code& ec) {
    // The first file path will be based on the namespace root namespace
    auto ns_path = path_for_namespace(params.root_namespace);
    // The first file paths:
    auto first_src    = params.directory / "src" / ns_path / (params.first_file_stem + ".cpp");
    auto first_header = params.directory / (params.separate_headers ? "include" : "src") / ns_path
        / (params.first_file_stem + ".hpp");

    // Prepare the include guard string by replacing non-ident elements with '_'
    auto guard = (ns_path.string() + "_" + params.first_file_stem + "_HPP_INCLUDED");
    boost::replace_all(guard, "/", "_");
    boost::replace_all(guard, "-", "_");
    boost::replace_all(guard, ".", "_");
    boost::to_upper(guard);

    // Set up the template render context
    kainjow::mustache::data ctx;
    ctx.set("root_ns", params.root_namespace);
    ctx.set("first_stem", params.first_file_stem);
    ctx.set("ns_path", ns_path.string());
    ctx.set("guard_def", guard);

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

    // Write the source file
    write_file(first_src, render("base/first_source.in.cpp"), ec);
    if (ec) {
        return;
    }

    // Write the header file
    write_file(first_header, render("base/first_header.in.hpp"), ec);
    if (ec) {
        return;
    }

    // Optional files
    if (params.create_examples) {
        write_file(params.directory / "examples/example1.cpp",
                   render("base/first_example.in.cpp"),
                   ec);
        if (ec) {
            return;
        }
    }

    if (params.create_tests) {
        write_file(params.directory / "tests/my_test.cpp", render("base/first_test.in.cpp"), ec);
        if (ec) {
            return;
        }
    }
}
