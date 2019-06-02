#ifndef PF_FS_ASCENDING_PATHS_HPP_INCLUDED
#define PF_FS_ASCENDING_PATHS_HPP_INCLUDED

#include <iterator>
#include <utility>

#include <pf/fs/core.hpp>

namespace pf {

class ascending_paths {
    class sentinel {};

    class iterator {
    public:
        using value_type        = fs::path;
        using reference         = value_type const&;
        using pointer           = value_type const*;
        using difference_type   = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;

        iterator() = default;

        iterator(sentinel)
            : iterator() {}

        explicit iterator(fs::path current)
            : current_{std::move(current)} {}

        iterator& operator++() {
            current_ = current_.parent_path();
            return *this;
        }

        iterator operator++(int) {
            auto cpy = *this;
            current_ = current_.parent_path();
            return cpy;
        }

        reference operator*() const { return current_; }
        pointer   operator->() const { return &**this; }

        friend bool operator==(iterator const& it, sentinel) {
            // ``path.parent_path()`` returns ``path`` if ``!has_relative_path()``,
            // which is how we know that we've reached the end.
            return !it.current_.has_relative_path();
        }

        friend bool operator==(sentinel end, iterator const& it) { return it == end; }
        friend bool operator!=(iterator const& it, sentinel end) { return !(it == end); }
        friend bool operator!=(sentinel end, iterator const& it) { return !(it == end); }

        friend bool operator==(iterator const& lhs, iterator const& rhs) {
            return lhs.current_ == rhs.current_;
        }
        friend bool operator!=(iterator const& lhs, iterator const& rhs) { return !(lhs == rhs); }

    private:
        fs::path current_ = {"/"};
    };

public:
    // Canonicalize, such that the path doesn't need to exist
    explicit ascending_paths(fs::path const& p)
        : current_{fs::weakly_canonical(p)} {}

    iterator begin() const { return iterator{current_}; }
    sentinel end() const { return sentinel{}; }

private:
    fs::path current_;
};

}  // namespace pf

#endif  // PF_FS_ASCENDING_PATHS_HPP_INCLUDED
