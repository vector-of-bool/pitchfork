#include "./files.hpp"

#include <pf/new/dirs.hpp>

#include <boost/algorithm/string.hpp>
#include <spdlog/fmt/ostr.h>

#include <fstream>

auto SRC_TEMPLATE = R"SRC(#include <{0}/{1}.hpp>

int {2}::calculate_value() {{
    // How many roads must a man walk down?
    return 6 * 7;
}}
)SRC";

auto HEADER_TEMPLATE = R"HEADER(#ifndef {1}
#define {1}

namespace {0} {{

/**
 * Calculate the answer. Not sure what the question is, though...
 */
int calculate_value();

}}

#endif // {1}
)HEADER";

void pf::create_files(const pf::new_project_params& params, std::error_code& ec) {
    // The first file path will be based on the namespace root namespace
    auto ns_path = path_for_namespace(params.root_namespace);
    // The first file paths:
    auto first_src    = params.directory / "src" / ns_path / (params.first_file_stem + ".cpp");
    auto first_header = params.directory / (params.separate_headers ? "include" : "src") / ns_path
        / (params.first_file_stem + ".hpp");

    // Write the source file
    auto content = fmt::format(SRC_TEMPLATE,
                               ns_path.string(),
                               params.first_file_stem,
                               params.root_namespace);
    write_file(first_src, content, ec);
    if (ec) {
        return;
    }

    // Write the header file
    // Prepare the include guard string by replacing non-ident elements with '_'
    auto guard = (ns_path.string() + "_" + params.first_file_stem + "_HPP_INCLUDED");
    boost::replace_all(guard, "/", "_");
    boost::replace_all(guard, "-", "_");
    boost::replace_all(guard, ".", "_");
    boost::to_upper(guard);
    content = fmt::format(HEADER_TEMPLATE, params.root_namespace, guard);
    write_file(first_header, content, ec);
    if (ec) {
        return;
    }
}
