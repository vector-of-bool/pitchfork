#include "./file_template.hpp"

#include <spdlog/fmt/ostr.h>

std::string pf::template_renderer::render(const std::string& inpath) const {
    auto res      = _fs.open(inpath);
    auto mustache = kainjow::mustache::mustache{{res.begin(), res.end()}};
    if (!mustache.is_valid()) {
        throw std::runtime_error(
            fmt::format("Error loading template file: {}: {}", inpath, mustache.error_message()));
    }
    return mustache.render(_template_data);
}
