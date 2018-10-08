#include "./fs.hpp"

#if STD_FS_IS_EXPERIMENTAL
#include <boost/filesystem.hpp>

namespace bf = boost::filesystem;
namespace bs = boost::system;
#endif

namespace fs = pf::fs;

#if STD_FS_IS_EXPERIMENTAL
fs::path fs::relative(fs::path const& p, std::error_code& ec) {
    bs::error_code bec;
    auto const     bf_result = bf::relative(bf::path{p}, bec);

    ec = std::make_error_code(static_cast<std::errc>(bec.value()));
    return fs::path{bf_result.c_str()};
}

fs::path fs::relative(fs::path const& p, fs::path const& base) {
    try {
        auto const bf_result = bf::relative(bf::path{p}, bf::path{base});
        return fs::path{bf_result.native()};
    } catch (bf::filesystem_error const& error) {
        throw fs::filesystem_error{
            error.what(),
            fs::path{error.path1().native()},
            fs::path{error.path2().native()},
            std::make_error_code(static_cast<std::errc>(error.code().value())),
        };
    }
}

fs::path fs::relative(fs::path const& p, fs::path const& base, std::error_code& ec) {
    bs::error_code bec;
    auto const     bf_result = bf::relative(bf::path{p}, bf::path{base}, bec);

    ec = std::make_error_code(static_cast<std::errc>(bec.value()));
    return fs::path{bf_result.native()};
}
#endif

std::fstream pf::open(const fs::path& filepath, std::ios::openmode mode, std::error_code& ec) {
    std::fstream ret;
    auto         mask = ret.exceptions() | std::ios::failbit;
    ret.exceptions(mask);

    try {
        ret.open(filepath.string(), mode);
    } catch (const std::ios::failure& e) {
        ec = e.code();
    }
    return std::move(ret);
}
