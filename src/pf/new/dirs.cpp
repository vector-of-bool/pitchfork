#include "./dirs.hpp"

#include <pf/new/project.hpp>

void pf::create_directories(const pf::new_project_params& params, std::error_code& ec) {
#define MK_SUBDIR(subdir)                                                                          \
    do {                                                                                           \
        fs::create_directories(params.directory / subdir, ec);                                     \
        if (ec) {                                                                                  \
            return;                                                                                \
        }                                                                                          \
    } while (0)
    // Create the root directory
    MK_SUBDIR(".");
    // Create the src/ directory
    MK_SUBDIR("src");
    // Create the include/ directory, if requested
    if (params.separate_headers) {
        MK_SUBDIR("include");
    }
}
