#ifndef PF_NEW_PARAMS_HPP_INCLUDED
#define PF_NEW_PARAMS_HPP_INCLUDED

#include <pf/fs.hpp>

namespace pf {

struct new_project_params {
    std::string name;
    fs::path    directory;
    std::string root_namespace;
};

}  // namespace pf

#endif  // PF_NEW_PARAMS_HPP_INCLUDED