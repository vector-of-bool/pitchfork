#include "./project.hpp"

#include <pf/new/dirs.hpp>
#include <pf/new/files.hpp>

void pf::create_project(const pf::new_project_params& params, std::error_code& ec) {
    pf::create_directories(params, ec);
    if (ec) {
        return;
    }
    pf::create_files(params, ec);
}
