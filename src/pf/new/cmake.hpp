#ifndef PF_NEW_CMAKE_HPP_INCLUDED
#define PF_NEW_CMAKE_HPP_INCLUDED

#include <pf/fs.hpp>
#include <pf/new/params.hpp>

namespace pf {

void create_cmake_files(const new_project_params& params, std::error_code& ec);

}  // namespace pf

#endif  // PF_NEW_CMAKE_HPP_INCLUDED