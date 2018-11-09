#ifndef PF_FS_GLOB_HPP_INCLUDED
#define PF_FS_GLOB_HPP_INCLUDED

#include <pf/fs/core.hpp>

namespace pf {
std::vector<fs::path> glob_sources(fs::path const& relative_to);
}

#endif  // PF_FS_GLOB_HPP_INCLUDED
