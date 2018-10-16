#ifndef PF_FS_ASCENDING_RANGE_HPP_INCLUDED
#define PF_FS_ASCENDING_RANGE_HPP_INCLUDED

#include <utility>

#include <boost/range/iterator_range.hpp>

#include <pf/fs/ascending_iterator.hpp>
#include <pf/fs/core.hpp>

namespace pf {

inline auto ascending_range(fs::path from, fs::path terminate = {}) {
    return boost::make_iterator_range(ascending_iterator{std::move(from)},
                                      ascending_iterator{std::move(terminate)});
}

}  // namespace pf

#endif  // PF_FS_ASCENDING_RANGE_HPP_INCLUDED
