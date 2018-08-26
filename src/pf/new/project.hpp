#ifndef PF_NEW_PROJECT_HPP_INCLUDED
#define PF_NEW_PROJECT_HPP_INCLUDED

#include <pf/fs.hpp>
#include <pf/new/params.hpp>

namespace pf {

fs::path    path_for_namespace(const std::string& ns);
void        create_project(const new_project_params& params, std::error_code& ec);
std::string namespace_for_name(const std::string& name);

}  // namespace pf

#endif  // PF_NEW_PROJECT_HPP_INCLUDED