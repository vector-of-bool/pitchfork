#include "./detect_base_dir.hpp"

#include <cstdlib>  // std::abort
#include <fstream>
#include <string>
#include <utility>

#include <boost/range/algorithm/find_if.hpp>

namespace fs = pf::fs;

namespace {
std::optional<fs::path> parse_cmakecache_homedir(fs::path const& cmakecache) {
    std::string line;

    std::ifstream file{cmakecache};

    while (std::getline(file, line)) {
        // if (line.starts_with("CMAKE_HOME_DIRECTORY"))
        if (line.rfind("CMAKE_HOME_DIRECTORY", 0) == 0) {
            auto index = line.find('=');
            line.erase(line.begin(), line.begin() + index + 1);
            return fs::path{line};
        }
    }

    return std::nullopt;
}
}  // namespace

std::optional<fs::path> pf::detect_base_dir(fs::path from_dir) {
    for (auto const& cur_dir : pf::ascending_range(from_dir)) {
        if (fs::exists(cur_dir / "CMakeLists.txt")) {
            // If we see a CMakeLists.txt, but the parent directory also has a CMakeLists.txt,
            // assume that we are a child of that parent directory
            return *boost::range::find_if(pf::ascending_range(cur_dir), [](auto const& path) {
                return !fs::exists(path.parent_path() / "CMakeLists.txt");
            });
        }

        fs::path const maybe_cmakecache = cur_dir / "CMakeCache.txt";

        if (fs::exists(maybe_cmakecache)) {
            return ::parse_cmakecache_homedir(maybe_cmakecache);
        }
    }

    return std::nullopt;
}
