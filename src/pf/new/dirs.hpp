#ifndef PF_NEW_DIRS_HPP_INCLUDED
#define PF_NEW_DIRS_HPP_INCLUDED

#include <pf/new/params.hpp>

namespace pf {

std::string namespace_for_name(const std::string& name);
fs::path path_for_namespace(const std::string& ns);
fs::path create_directories(const new_project_params& params, std::error_code& ec);

}  // namespace pf

#endif  // PF_NEW_DIRS_HPP_INCLUDED