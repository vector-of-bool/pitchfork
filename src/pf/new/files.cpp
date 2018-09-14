#include "./files.hpp"

#include <pf/file_template.hpp>
#include <pf/new/dirs.hpp>
#include <pf/new/project.hpp>

#include <cmrc/cmrc.hpp>

#include <boost/algorithm/string.hpp>
#include <kainjow/mustache.hpp>

#include <fstream>

CMRC_DECLARE(pf_templates);

void pf::create_files(const pf::new_project_params& params) {
    pf::template_renderer trr{params.directory, cmrc::pf_templates::get_filesystem()};
    // The first file path will be based on the namespace root namespace
    auto ns_path = path_for_namespace(params.root_namespace);
    // The first file paths:
    auto first_src    = "src" / ns_path / (params.first_file_stem + ".cpp");
    auto first_header = (params.separate_headers ? "include" : "src") / ns_path
        / (params.first_file_stem + ".hpp");

    // Prepare the include guard string by replacing non-ident elements with '_'
    auto guard = (ns_path.string() + "_" + params.first_file_stem + "_HPP_INCLUDED");
    boost::replace_all(guard, "/", "_");
    boost::replace_all(guard, "-", "_");
    boost::replace_all(guard, ".", "_");
    boost::to_upper(guard);

    // Set up the template render context
    trr.set("root_ns", params.root_namespace);
    trr.set("first_stem", params.first_file_stem);
    trr.set("ns_path", ns_path.string());
    trr.set("guard_def", guard);

    // Base files
    trr.render_to_file("base/first_source.in.cpp", first_src);
    trr.render_to_file("base/first_header.in.hpp", first_header);

    // Optional files
    if (params.create_examples) {
        trr.render_to_file("base/first_example.in.cpp", "examples/example1.cpp");
    }

    if (params.create_tests) {
        trr.render_to_file("base/first_test.in.cpp", "tests/my_test.cpp");
    }
}
