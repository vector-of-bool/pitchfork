#ifndef PF_NEW_FILES_HPP_INCLUDED
#define PF_NEW_FILES_HPP_INCLUDED

#include <pf/new/params.hpp>

namespace pf {

void create_files(const pf::new_project_params& params, std::error_code& ec);

}  // namespace pf

#endif  // PF_NEW_FILES_HPP_INCLUDED