#ifndef PF_EXISTING_DETECT_PROJECT_ROOT_HPP_INCLUDED
#define PF_EXISTING_DETECT_PROJECT_ROOT_HPP_INCLUDED

#include <boost/optional.hpp>

#include <pf/fs.hpp>

namespace pf {

boost::optional<fs::path> detect_project_root(fs::path from_dir = fs::current_path());

}  // namespace pf

#endif  // PF_EXISTING_DETECT_PROJECT_ROOT_HPP_INCLUDED
