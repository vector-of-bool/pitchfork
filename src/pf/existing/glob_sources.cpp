#include "./glob_sources.hpp"

#include <algorithm>
#include <iterator>
#include <unordered_set>

namespace fs = pf::fs;

namespace {
struct path_hash {
    auto operator()(fs::path const& path) const {
        return fs::hash_value(path);  //
    }
};

template <typename Pred>
std::vector<fs::path> glob_impl(fs::path const& directory, Pred pred) {
    std::vector<fs::path> result;

    std::copy_if(fs::recursive_directory_iterator{directory},
                 fs::recursive_directory_iterator{},
                 std::back_inserter(result),
                 pred);

    return result;
}
}  // namespace

std::vector<fs::path> pf::glob_sources(fs::path const& directory) {
    static std::unordered_set<fs::path, path_hash> SourceFileExtensions{
        fs::path{".c"},
        fs::path{".cc"},
        fs::path{".cpp"},
        fs::path{".cxx"},
        fs::path{".c++"},
    };

    return ::glob_impl(directory, [&](fs::directory_entry const& entry) {
        return SourceFileExtensions.count(entry.path().extension());
    });
}

std::vector<fs::path> pf::glob_headers(fs::path const& directory) {
    static std::unordered_set<fs::path, path_hash> HeaderFileExtensions{
        fs::path{".h"},
        fs::path{".hh"},
        fs::path{".hpp"},
        fs::path{".hxx"},
        fs::path{".h++"},
    };

    return ::glob_impl(directory, [&](fs::directory_entry const& entry) {
        return HeaderFileExtensions.count(entry.path().extension());
    });
}
