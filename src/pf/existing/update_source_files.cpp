#include "./update_source_files.hpp"

#include <fstream>
#include <sstream>

#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

namespace fs = pf::fs;

void pf::update_source_files(fs::path const&           project_root,
                             std::set<fs::path> const& headers,
                             std::set<fs::path> const& sources) {
    fs::path const src_cmakelists = project_root / "src/CMakeLists.txt";

    if (fs::exists(src_cmakelists)) {
        std::ifstream cmakelists_file{src_cmakelists};
        std::string   cmakelists = [&] {
            std::ostringstream out;
            out << cmakelists_file.rdbuf();
            return out.str();
        }();

        (void)cmakelists;
        (void)headers;
        (void)sources;
    } else {
        spdlog::get("console")->warn("No CMakeLists.txt found at: {}", src_cmakelists);
    }
}
