#include "./core.hpp"

namespace fs = pf::fs;

std::fstream pf::open(const fs::path& filepath, std::ios::openmode mode, std::error_code& ec) {
    std::fstream ret;
    auto         mask = ret.exceptions() | std::ios::failbit;
    ret.exceptions(mask);

    try {
        ret.open(filepath.string(), mode);
    } catch (const std::ios::failure& e) {
        ec = e.code();
    }
    return ret;
}
