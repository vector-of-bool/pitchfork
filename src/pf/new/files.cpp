#include "./files.hpp"

#include <pf/new/dirs.hpp>
#include <pf/new/project.hpp>

#include <boost/algorithm/string.hpp>
#include <kainjow/mustache.hpp>

#include <fstream>

namespace {
auto SRC_TEMPLATE = R"SRC(#include <{{src_dir}}/{{first_stem}}.hpp>

int {{root_ns}}::calculate_value() {
    // How many roads must a man walk down?
    return 6 * 7;
}
)SRC";

auto HEADER_TEMPLATE = R"HEADER(#ifndef {{guard_def}}
#define {{guard_def}}

namespace {{root_ns}} {

/**
 * Calculate the answer. Not sure what the question is, though...
 */
int calculate_value();

}

#endif // {{guard_def}}
)HEADER";

auto EXAMPLE1_CPP = R"(#include <iostream>

#include <{{src_dir}}/{{first_stem}}.hpp>

int main() {
    std::cout << "I am an example executable\n";
    std::cout << "Let's calculate the value...\n";
    const auto value = {{root_ns}}::calculate_value();
    std::cout << "The value we got is " << value << '\n';
}
)";
}  // namespace

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
    ctx.set("src_dir", ns_path.string());
    ctx.set("guard_def", guard);

    // Write the source file
    auto content = kainjow::mustache::mustache{SRC_TEMPLATE}.render(ctx);
    write_file(first_src, content, ec);
    if (ec) {
        return;
    }

    // Write the header file
    content = kainjow::mustache::mustache{HEADER_TEMPLATE}.render(ctx);
    write_file(first_header, content, ec);
    if (ec) {
        return;
    }

    // Optional files
    if (params.create_examples) {
        content = kainjow::mustache::mustache{EXAMPLE1_CPP}.render(ctx);
        write_file(params.directory / "examples/example1.cpp", content, ec);
        if (ec) {
            return;
        }
    }
}
