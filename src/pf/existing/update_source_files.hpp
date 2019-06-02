#ifndef PF_EXISTING_UPDATE_SOURCE_FILES_HPP_INCLUDED
#define PF_EXISTING_UPDATE_SOURCE_FILES_HPP_INCLUDED

#include <vector>

#include <pf/fs.hpp>

namespace pf {

enum class update_grouping {
    unspecified,
    none,
    smart,
};

void update_source_files(fs::path const&              cmakelists_file,
                         std::vector<fs::path> const& sources,
                         update_grouping              grouping = update_grouping::smart);

}  // namespace pf

#endif  // PF_EXISTING_UPDATE_SOURCE_FILES_HPP_INCLUDED
