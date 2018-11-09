#ifndef PF_FS_ASCENDING_ITERATOR_HPP_INCLUDED
#define PF_FS_ASCENDING_ITERATOR_HPP_INCLUDED

#include <iterator>
#include <utility>

#include <pf/fs/core.hpp>

namespace pf {

class ascending_iterator {
public:
    using difference_type   = std::ptrdiff_t;
    using value_type        = fs::path;
    using pointer           = value_type const*;
    using reference         = value_type const&;
    using iterator_category = std::input_iterator_tag;

    ascending_iterator() = default;

    // Canonicalizing the path makes the `==` comparison valid
    explicit ascending_iterator(fs::path const& p)
        : current_{fs::weakly_canonical(p)} {}

    ascending_iterator& operator++() {
        current_ = current_.parent_path();
        return *this;
    }

    ascending_iterator operator++(int) {
        auto cpy = *this;
        ++*this;
        return cpy;
    }

    reference operator*() const { return current_; }
    pointer   operator->() const { return &current_; }

    friend bool operator==(ascending_iterator const& lhs, ascending_iterator const& rhs) {
        return lhs.current_ == rhs.current_;
    }

    friend bool operator!=(ascending_iterator const& lhs, ascending_iterator const& rhs) {
        return !(lhs == rhs);
    }

private:
    fs::path current_ = {"/"};
};

}  // namespace pf

#endif  // PF_FS_ASCENDING_ITERATOR_HPP_INCLUDED
