#include "./dirs.hpp"

#include <boost/algorithm/string.hpp>

std::string pf::namespace_for_name(const std::string& name) {
    auto ns = name;
    boost::replace_all(ns, "-", "::");
    boost::replace_all(ns, ".", "::");
    boost::replace_all(ns, "/", "::");
    return ns;
}

pf::fs::path pf::path_for_namespace(const std::string& ns) {
    return boost::replace_all_copy(ns, "::", "/");
}

pf::fs::path pf::create_directories(const pf::new_project_params& params, std::error_code& ec) {
#define MK_SUBDIR(subdir)                                                                          \
    do {                                                                                           \
        fs::create_directories(params.directory / subdir, ec);                                     \
        if (ec) {                                                                                  \
            return params.directory;                                                               \
        }                                                                                          \
    } while (0)
    // Create the root directory
    MK_SUBDIR(".");
    // Create the src/ directory
    auto ns_subdir = path_for_namespace(params.root_namespace);
    MK_SUBDIR("src" / ns_subdir);
    // Create the include/ directory, if requested
    if (params.separate_headers) {
        MK_SUBDIR("include" / ns_subdir);
    }
    return fs::canonical(params.directory);
}
