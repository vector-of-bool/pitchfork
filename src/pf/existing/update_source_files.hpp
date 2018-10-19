#ifndef PF_EXISTING_UPDATE_SOURCE_FILES_HPP_INCLUDED
#define PF_EXISTING_UPDATE_SOURCE_FILES_HPP_INCLUDED

#include <vector>

#include <pf/fs.hpp>

namespace pf {

void update_source_files(fs::path const& project_root, std::vector<fs::path> const& sources);

}  // namespace pf

#endif  // PF_EXISTING_UPDATE_SOURCE_FILES_HPP_INCLUDED
