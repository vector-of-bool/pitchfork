#include "./detect_base_dir.hpp"

#include <algorithm>
#include <fstream>
#include <string>
#include <utility>

#include <boost/range/iterator_range.hpp>

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
    auto cur_dir = std::find_if(pf::ascending_iterator{from_dir},
                                pf::ascending_iterator{},
                                [](auto const& dir) {
                                    return fs::exists(dir / "CMakeLists.txt")
                                        || fs::exists(dir / "CMakeCache.txt");
                                });

    if (cur_dir == pf::ascending_iterator{}) {
        return std::nullopt;
    }

    if (fs::exists(*cur_dir / "CMakeLists.txt")) {
        return *std::find_if(pf::ascending_iterator{*cur_dir},
                             pf::ascending_iterator{},
                             [](auto const& path) {
                                 return !fs::exists(path.parent_path() / "CMakeLists.txt");
                             });
    }

    // There's a CMakeCache
    return ::parse_cmakecache_homedir(*cur_dir / "CMakeCache.txt");
}
