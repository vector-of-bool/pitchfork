#ifndef PF_NEW_PARAMS_HPP_INCLUDED
#define PF_NEW_PARAMS_HPP_INCLUDED

#include <pf/fs.hpp>

namespace pf {

enum class build_system {
    unspecified, // Used by the CLI when nothing has been specified
    none,
    cmake,
};

struct new_project_params {
    std::string       name;
    fs::path          directory;
    std::string       root_namespace;
    bool              separate_headers   = false;
    bool              create_third_party = true;
    bool              create_examples    = true;
    bool              create_extras      = false;
    std::string       first_file_stem;
    enum build_system build_system = build_system::none;
};

}  // namespace pf

#endif  // PF_NEW_PARAMS_HPP_INCLUDED