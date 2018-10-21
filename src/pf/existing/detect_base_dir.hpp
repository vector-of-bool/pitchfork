#ifndef PF_EXISTING_DETECT_BASE_DIR_HPP_INCLUDED
#define PF_EXISTING_DETECT_BASE_DIR_HPP_INCLUDED

#include <optional>

#include <pf/fs.hpp>

namespace pf {

std::optional<fs::path> detect_base_dir(fs::path from_dir = fs::current_path());

}  // namespace pf

#endif  // PF_EXISTING_DETECT_BASE_DIR_HPP_INCLUDED
