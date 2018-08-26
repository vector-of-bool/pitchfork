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
    auto ns_path      = path_for_namespace(params.root_namespace);
    auto first_src    = params.directory / "src" / ns_path / (params.first_file_stem + ".cpp");
    auto first_header = params.directory / (params.separate_headers ? "include" : "src") / ns_path
        / (params.first_file_stem + ".hpp");
    auto ofile = pf::open(first_src, std::ios::out | std::ios::binary, ec);
    if (ec) {
        return;
    }
    ofile << fmt::format(SRC_TEMPLATE, ns_path, params.first_file_stem, params.root_namespace);
    ofile = pf::open(first_header, std::ios::out | std::ios::binary, ec);
    if (ec) {
        return;
    }
    auto guard = (ns_path.string() + "_" + params.first_file_stem + "_HPP_INCLUDED");
    boost::replace_all(guard, "/", "_");
    boost::replace_all(guard, "-", "_");
    boost::replace_all(guard, ".", "_");
    boost::to_upper(guard);
    ofile << fmt::format(HEADER_TEMPLATE, params.root_namespace, guard);
}
