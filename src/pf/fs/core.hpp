#ifndef PF_FS_CORE_HPP_INCLUDED
#define PF_FS_CORE_HPP_INCLUDED

#include <fstream>
#include <system_error>

#if STD_FS_IS_EXPERIMENTAL
#include <experimental/filesystem>
namespace pf {

namespace fs {
using namespace ::std::experimental::filesystem;

// std::experimental::filesystem doesn't have fs::relative
fs::path relative(fs::path const& p, std::error_code& ec);

fs::path relative(fs::path const& p, fs::path const& base = fs::current_path());

fs::path relative(fs::path const& p, fs::path const& base, std::error_code& ec);
}  // namespace fs

}  // namespace pf
#else
#include <filesystem>
namespace pf {

namespace fs {
using namespace ::std::filesystem;
}

}  // namespace pf
#endif

namespace pf {

/**
 * Open the given filepath with the given openmode. Fills out `ec` with an error code in case of
 * file open failure.
 */
std::fstream open(const fs::path&, std::ios::openmode, std::error_code& ec);
/**
 * Open the given filepath, but throw std::system_error in case of failure to open
 */
inline std::fstream open(const fs::path& filepath, std::ios::openmode mode) {
    std::error_code ec;
    auto            ret = open(filepath, mode, ec);
    if (ec) {
        throw std::system_error{ec, "Open file: " + filepath.string()};
    }
    return std::move(ret);
}

/**
 * Write the contents of a file, creating parents directories if necessary.
 */
template <typename What>
void write_file(const fs::path& path, What&& w, std::error_code& ec) {
    fs::create_directories(path.parent_path(), ec);
    if (ec) {
        return;
    }
    auto strm = open(path, std::ios::out | std::ios::binary, ec);
    if (ec) {
        return;
    }
    strm << std::forward<What>(w);
    ec = {};
}

template <typename What>
void write_file(const fs::path& path, What&& w) {
    std::error_code ec;
    write_file(path, std::forward<What>(w), ec);
    if (ec) {
        throw std::system_error{ec, "Failed to write file: " + path.string()};
    }
}
}  // namespace pf

#endif  // PF_FS_CORE_HPP_INCLUDED