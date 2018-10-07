#include "./detect_project_root.hpp"

#include <cstdlib>  // std::abort
#include <fstream>
#include <string>
#include <utility>

namespace fs = pf::fs;

namespace {
boost::optional<fs::path> parse_cmakecache_homedir(fs::path const& cmakecache) {
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

    return boost::none;
}
}  // namespace

boost::optional<fs::path> pf::detect_project_root(fs::path cur_dir) {
    for (; cur_dir.has_parent_path(); cur_dir = cur_dir.parent_path()) {
        if (fs::exists(cur_dir / "CMakeLists.txt")) {
            // If we see a CMakeLists.txt, but the parent directory also has a CMakeLists.txt,
            // assume that we are a child of that parent directory
            for (fs::path parent = cur_dir.parent_path(); fs::exists(parent / "CMakeLists.txt");
                 cur_dir         = std::exchange(parent, parent.parent_path())) {
            }

            return cur_dir;
        }

        fs::path const maybe_cmakecache = cur_dir / "CMakeCache.txt";

        if (fs::exists(maybe_cmakecache)) {
            return ::parse_cmakecache_homedir(maybe_cmakecache);
        }
    }

    return boost::none;
}
