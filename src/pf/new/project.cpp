#include "./project.hpp"

#include <pf/new/cmake.hpp>
#include <pf/new/dirs.hpp>
#include <pf/new/files.hpp>

#include <boost/algorithm/string.hpp>

pf::fs::path pf::path_for_namespace(const std::string& ns) {
    return boost::replace_all_copy(ns, "::", "/");
}

std::string pf::namespace_for_name(const std::string& name) {
    auto ns = name;
    boost::replace_all(ns, "-", "::");
    boost::replace_all(ns, ".", "::");
    boost::replace_all(ns, "/", "::");
    return ns;
}

void pf::create_project(const pf::new_project_params& params, std::error_code& ec) {
    pf::create_directories(params, ec);
    if (ec) {
        return;
    }
    pf::create_files(params, ec);
    if (ec) {
        return;
    }
    if (params.build_system == pf::build_system::cmake) {
        pf::create_cmake_files(params, ec);
        if (ec) {
            return;
        }
    }
}
