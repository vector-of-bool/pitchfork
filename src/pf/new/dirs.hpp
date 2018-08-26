#ifndef PF_NEW_DIRS_HPP_INCLUDED
#define PF_NEW_DIRS_HPP_INCLUDED

#include <pf/new/params.hpp>

namespace pf {

void create_directories(const new_project_params& params, std::error_code& ec);

}  // namespace pf

#endif  // PF_NEW_DIRS_HPP_INCLUDED