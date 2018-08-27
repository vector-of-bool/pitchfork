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
    // Required subdirectories:
    MK_SUBDIR("src");
    MK_SUBDIR("tests");
    MK_SUBDIR("docs");
    // Conditional subdirs:
    if (params.separate_headers) {
        MK_SUBDIR("include");
    }
    if (params.create_third_party) {
        MK_SUBDIR("third_party");
    }
    if (params.create_examples) {
        MK_SUBDIR("examples");
    }
    if (params.create_extras) {
        MK_SUBDIR("extras");
    }
    // Build system
    if (params.build_system == pf::build_system::cmake) {
        MK_SUBDIR("cmake");
    }
}
