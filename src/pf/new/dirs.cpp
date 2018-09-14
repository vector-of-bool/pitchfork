#include "./dirs.hpp"

#include <pf/new/project.hpp>

void pf::create_directories(const pf::new_project_params& params) {
    // Create the root directory
    fs::create_directories(params.directory);
    // Required subdirectories:
    fs::create_directories(params.directory / "src");
    fs::create_directories(params.directory / "docs");
    // Conditional subdirs:
    if (params.separate_headers) {
        fs::create_directories(params.directory / "include");
    }
    if (params.create_third_party) {
        fs::create_directories(params.directory / "third_party");
    }
    if (params.create_examples) {
        fs::create_directories(params.directory / "examples");
    }
    if (params.create_extras) {
        fs::create_directories(params.directory / "extras");
    }
    if (params.create_tests) {
        fs::create_directories(params.directory / "tests");
    }
    // Build system
    if (params.build_system == pf::build_system::cmake) {
        fs::create_directories(params.directory / "cmake");
    }
}
