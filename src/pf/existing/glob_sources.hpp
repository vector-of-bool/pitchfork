#ifndef PF_EXISTING_GLOB_SOURCES_HPP_INCLUDED
#define PF_EXISTING_GLOB_SOURCES_HPP_INCLUDED

#include <set>

#include <pf/fs.hpp>

namespace pf {

// TODO: decide if it's worth computing this lazily
std::set<fs::path> glob_sources(fs::path const& directory);

// TODO: should this be part of `glob_sources`, or should there be a `glob_sources_and_headers`?
std::set<fs::path> glob_headers(fs::path const& directory);

}  // namespace pf

#endif  // PF_EXISTING_GLOB_SOURCES_HPP_INCLUDED
