#include "./compare_fs.hpp"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iterator>
#include <string_view>

using namespace pf;
using namespace pf::test;

namespace {

// Filename to ignore in diffs. Allows us to have "empty" directories in git
constexpr std::string_view IgnoreDiff = "ignore_in_diff";

using path_set = std::set<fs::path>;
using dir_iter = pf::fs::directory_iterator;

path_set children(fs::path basis, fs::path path) {
    path_set        ret;
    std::error_code ec;
    auto            iter = fs::directory_iterator{path, ec};
    if (ec) {
        if (ec == std::errc::no_such_file_or_directory) {
            return ret;
        }
        throw std::system_error{ec, "Cannot get children of non-directory file: " + path.string()};
    }
    for (fs::path child : iter) {
        if (child.stem().string() == IgnoreDiff) {
            continue;
        }
        ret.insert(fs::relative(child, basis));
    }
    return ret;
}

template <typename Container>
decltype(auto) difference(Container&& lhs, Container&& rhs) {
    std::decay_t<Container> ret;
    std::set_difference(lhs.begin(),
                        lhs.end(),
                        rhs.begin(),
                        rhs.end(),
                        std::inserter(ret, ret.begin()));
    return ret;
}

void acc_differences(fs_diff&        out,
                     const fs::path& in_root,
                     const fs::path& exp_root,
                     const fs::path  child) {
    auto in_files   = children(in_root, in_root / child);
    auto exp_files  = children(exp_root, exp_root / child);
    auto unexpected = difference(in_files, exp_files);
    auto missing    = difference(exp_files, in_files);
    out.unexpected_files.insert(unexpected.begin(), unexpected.end());
    out.missing_files.insert(missing.begin(), missing.end());

    path_set both_children;
    std::set_intersection(in_files.begin(),
                          in_files.end(),
                          exp_files.begin(),
                          exp_files.end(),
                          std::inserter(both_children, both_children.begin()));
    for (auto child : both_children) {
        auto in_stat  = fs::status(in_root / child);
        auto exp_stat = fs::status(exp_root / child);
        if (in_stat.type() != exp_stat.type()) {
            out.different_files.insert(child);
        } else if (fs::is_regular_file(in_stat)) {
            assert(fs::is_regular_file(exp_stat));
            std::ifstream in_strm{in_root / child, std::ios::binary};
            std::ifstream exp_strm{exp_root / child, std::ios::binary};
            using char_iter = std::istreambuf_iterator<char>;
            bool are_equal
                = std::equal(char_iter{in_strm}, char_iter{}, char_iter{exp_strm}, char_iter{});
            if (!are_equal) {
                out.different_files.insert(child);
            }
        }
    }

    path_set all_children = in_files;
    all_children.insert(exp_files.begin(), exp_files.end());
    for (auto& child : all_children) {
        if (fs::is_directory(in_root / child) || fs::is_directory(exp_root / child)) {
            acc_differences(out, in_root, exp_root, child);
        }
    }
}

}  // namespace

pf::test::fs_diff pf::test::compare_fs_tree(pf::fs::path input, pf::fs::path expected) {
    fs_diff ret;
    acc_differences(ret, input, expected, "");
    return ret;
}
