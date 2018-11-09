#include "./glob.hpp"

#include <algorithm>
#include <iterator>
#include <unordered_set>

namespace fs = pf::fs;

std::vector<fs::path> pf::glob_sources(fs::path const& relative_to) {
    std::vector<fs::path> sources;

    struct path_hash {
        auto operator()(fs::path const& path) const {
            return fs::hash_value(path);  //
        }
    };
    std::unordered_set<fs::path, path_hash> const SourceFileExtensions{
        fs::path{".c"},
        fs::path{".cc"},
        fs::path{".cpp"},
        fs::path{".cxx"},
        fs::path{".c++"},
        fs::path{".h"},
        fs::path{".hh"},
        fs::path{".hpp"},
        fs::path{".hxx"},
        fs::path{".h++"},
    };

    std::for_each(fs::directory_iterator{relative_to},
                  fs::directory_iterator{},
                  [&](fs::directory_entry const& top_level_entry) {
                      if (top_level_entry.is_directory()) {
                          std::copy_if(fs::recursive_directory_iterator{top_level_entry.path()},
                                       fs::recursive_directory_iterator{},
                                       std::back_inserter(sources),
                                       [&](fs::directory_entry const& entry) {
                                           return SourceFileExtensions.count(
                                               entry.path().extension());
                                       });
                      }
                  });

    std::sort(sources.begin(), sources.end());

    return sources;
}
