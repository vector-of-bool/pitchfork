#include "./detect_base_dir.hpp"

#include <algorithm>
#include <fstream>
#include <string>
#include <utility>

#include <range/v3/algorithm/find_if.hpp>

namespace fs = pf::fs;

namespace {
std::optional<fs::path> parse_cmakecache_homedir(fs::path const& cmakecache) {
    std::string line;

    std::fstream file = pf::open(cmakecache, std::ios::in);

    while (std::getline(file, line)) {
        // Equivalent to the non-existent:
        // if (line.starts_with("..."))
        if (line.rfind("CMAKE_HOME_DIRECTORY:", 0) == 0) {
            auto index = line.find('=');
            line.erase(line.begin(), line.begin() + index + 1);
            return fs::path{line};
        }
    }

    return std::nullopt;
}
}  // namespace

std::optional<fs::path> pf::detect_base_dir(fs::path from_dir) {
    auto ascending_from = pf::ascending_paths(from_dir);
    auto cur_dir        = ranges::find_if(ascending_from, [](auto const& dir) {
        return fs::exists(dir / "CMakeLists.txt") || fs::exists(dir / "CMakeCache.txt");
    });

    // Iterator always valid, but may be root dir `/`.
    if (fs::exists(*cur_dir / "CMakeLists.txt")) {
        auto ascending_cur = pf::ascending_paths(*cur_dir);
        return *ranges::find_if(ascending_cur, [](auto const& path) {
            return !fs::exists(path.parent_path() / "CMakeLists.txt");
        });
    } else if (fs::exists(*cur_dir / "CMakeCache.txt")) {
        return ::parse_cmakecache_homedir(*cur_dir / "CMakeCache.txt");
    } else {
        return std::nullopt;
    }
}
