#ifndef PF_FS_ASCENDING_RANGE_HPP_INCLUDED
#define PF_FS_ASCENDING_RANGE_HPP_INCLUDED

#include <boost/range/iterator_range.hpp>

#include <pf/fs/ascending_iterator.hpp>
#include <pf/fs/core.hpp>

namespace pf {

inline auto ascending_range(fs::path const& from, fs::path const& terminate = {}) {
    return boost::make_iterator_range(ascending_iterator{from}, ascending_iterator{terminate});
}

}  // namespace pf

#endif  // PF_FS_ASCENDING_RANGE_HPP_INCLUDED
