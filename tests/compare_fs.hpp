#ifndef COMPARE_FS_HPP_INCLUDED
#define COMPARE_FS_HPP_INCLUDED

#include <pf/fs.hpp>

#include <iosfwd>
#include <set>

namespace pf::test {

class fs_diff {
public:
    std::set<fs::path> unexpected_files;
    std::set<fs::path> missing_files;
    std::set<fs::path> different_files;
    explicit           operator bool() const noexcept {
        return !unexpected_files.empty() || !missing_files.empty() || !different_files.empty();
    }
};

fs_diff compare_fs_tree(fs::path input, fs::path expected);

inline std::ostream& operator<<(std::ostream& o, const fs_diff& diff) {
    if (!diff.unexpected_files.empty()) {
        o << "Unexpected files in output directory:\n";
        for (auto& child : diff.unexpected_files) {
            o << "  + " << child.string() << '\n';
        }
    }
    if (!diff.missing_files.empty()) {
        o << "Files missing from the output directory:\n";
        for (auto& child : diff.missing_files) {
            o << "  - " << child.string() << '\n';
        }
    }
    if (!diff.different_files.empty()) {
        o << "Files that are not equivalent:\n";
        for (auto& child : diff.different_files) {
            o << " != " << child.string() << '\n';
        }
    }
    return o;
}

}  // namespace pf::test

#endif  // COMPARE_FS_HPP_INCLUDED