#ifndef PF_EXISTING_DETECT_ROOT_HPP_INCLUDED
#define PF_EXISTING_DETECT_ROOT_HPP_INCLUDED

#include <pf/fs.hpp>

namespace pf {

// TODO: evaluate possible performance impact from calling fs::current_path() each time
fs::path detect_project_root(fs::path cur_dir = fs::current_path());

}  // namespace pf

#endif  // PF_EXISTING_DETECT_ROOT_HPP_INCLUDED
