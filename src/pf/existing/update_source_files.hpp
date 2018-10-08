#ifndef PF_EXISTING_UPDATE_SOURCE_FILES_HPP_INCLUDED
#define PF_EXISTING_UPDATE_SOURCE_FILES_HPP_INCLUDED

#include <set>

#include <pf/fs.hpp>

namespace pf {

void update_source_files(fs::path const&           project_root,
                         std::set<fs::path> const& headers,
                         std::set<fs::path> const& sources);

}  // namespace pf

#endif  // PF_EXISTING_UPDATE_SOURCE_FILES_HPP_INCLUDED
