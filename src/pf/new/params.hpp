#ifndef PF_NEW_PARAMS_HPP_INCLUDED
#define PF_NEW_PARAMS_HPP_INCLUDED

#include <pf/fs.hpp>

namespace pf {

enum class build_system {
    unspecified,  // Used by the CLI when nothing has been specified
    none,
    cmake,
};

struct new_project_params {
    std::string       name;
    std::string       root_namespace;
    std::string       first_file_stem;
    fs::path          directory;
    bool              separate_headers   = false;
    bool              create_third_party = true;
    bool              create_examples    = true;
    bool              create_extras      = false;
    bool              create_tests       = true;
    enum build_system build_system       = build_system::none;

    new_project_params(std::string name_,
                       std::string root_ns,
                       std::string first_file_stem_,
                       fs::path    directory_)
        : name(std::move(name_))
        , root_namespace(std::move(root_ns))
        , first_file_stem(std::move(first_file_stem_))
        , directory(directory_) {}
};

}  // namespace pf

#endif  // PF_NEW_PARAMS_HPP_INCLUDED