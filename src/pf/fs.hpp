#ifndef PF_FS_HPP_INCLUDED
#define PF_FS_HPP_INCLUDED

#if STD_FS_IS_EXPERIMENTAL
#include <experimental/filesystem>
namespace pf {

namespace fs = std::experimental::filesystem;

} // namespace pf
#else
#include <filesystem>
namespace pf {

namespace fs = std::experimental::filesystem;

} // namespace pf
#endif

#endif // PF_FS_HPP_INCLUDED