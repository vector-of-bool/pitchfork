#include "./core.hpp"

#include <sstream>

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

std::string pf::slurp_file(const fs::path& path, std::error_code& ec) {
    auto file = pf::open(path, std::ios::in, ec);
    if (ec) {
        return std::string{};
    }

    std::ostringstream out;
    out << file.rdbuf();
    return std::move(out).str();
}
