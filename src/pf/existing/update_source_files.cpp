#include "./update_source_files.hpp"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <fstream>
#include <iterator>
#include <sstream>
#include <tuple>
#include <utility>

#include <range/v3/view/group_by.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/transform.hpp>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

namespace view = ranges::view;
namespace fs   = pf::fs;

using namespace std::literals;

namespace {
auto transform_relative(fs::path const& base_dir) {
    return view::transform([&base_dir](auto const& it) {
        auto result = fs::relative(it, base_dir).string();
        // TODO: evaluate if replacing `\` in filenames might cause a problem
        std::replace(result.begin(), result.end(), '\\', '/');
        return result;
    });
}

constexpr bool smart_grouping = true;

std::vector<std::string> relative_source_strings(std::vector<fs::path> const& source_files,
                                                 fs::path const&              base_dir,
                                                 pf::update_grouping          grouping) {
    assert(grouping == pf::update_grouping::smart || grouping == pf::update_grouping::none);

    auto result = source_files | ::transform_relative(base_dir);

    if (grouping == pf::update_grouping::smart) {
        return result | view::group_by([](std::string const& lhs, std::string const& rhs) {
                   // Note: computes each length at least twice rather than just once.
                   // Much simpler than combining with view::zip_with, though.
                   auto const lhs_dir_len = lhs.rfind('/');
                   auto const rhs_dir_len = rhs.rfind('/');

                   if (lhs_dir_len == std::string::npos) {
                       return lhs_dir_len == rhs_dir_len;
                   }

                   return std::string_view{lhs.c_str(), lhs_dir_len}
                   == std::string_view{rhs.c_str(), rhs_dir_len};
               })
            | view::join(""s);
    }

    return result;
}

constexpr std::string_view SourcesComment = "# sources\n";

auto find_next_function(std::string::iterator begin, std::string::iterator end) {
    // BUG: fails for ( in a comment.
    // TODO: use a proper CMake parser, or otherwise improve this
    return std::find(begin, end, '(');
}

auto find_end_function(std::string::iterator begin, std::string::iterator end) {
    return std::find(begin, end, ')');
}

// Assumes we are in the root location of a CMakeLists.txt
auto find_insertion_indicator_comment(std::string::iterator begin_fn,
                                      std::string::iterator end_fn) {
    auto const insertion_comment
        = std::search(begin_fn, end_fn, SourcesComment.begin(), SourcesComment.end());

    return insertion_comment;
}

auto get_indent_at_indicator_comment(std::string::iterator                  insertion_comment,
                                     std::string::iterator                  begin_fn,
                                     [[maybe_unused]] std::string::iterator end_fn) {
    auto const begin_of_line = std::find_if(std::make_reverse_iterator(insertion_comment),
                                            std::make_reverse_iterator(begin_fn),
                                            [](char c) { return c == '\n' || !std::isspace(c); })
                                   .base();

    auto indent_begin = begin_of_line;
    auto indent_end   = insertion_comment;

    return std::string{indent_begin, indent_end};
}

auto find_insertion_location(std::string::iterator                  insertion_comment,
                             [[maybe_unused]] std::string::iterator begin_fn,
                             [[maybe_unused]] std::string::iterator end_fn) {
    return std::next(insertion_comment, SourcesComment.size());
}

auto erase_existing_sources(std::string&          cmakelists,
                            std::string::iterator insertion_point,
                            std::string::iterator begin_fn,
                            std::string::iterator end_fn) {
    // We don't delete until the end of the ')', but until the last source-list character,
    // meaning:
    // ...
    // # sources
    // something
    //  )
    // ... Gets turned into:
    // # sources
    //  )
    auto last_source_list_char = std::find_if_not(std::make_reverse_iterator(end_fn),
                                                  std::make_reverse_iterator(begin_fn),
                                                  [](char c) { return std::isblank(c); });
    if (*last_source_list_char == '\n') {
        ++last_source_list_char;
    }

    if (last_source_list_char.base() <= insertion_point) {
        // If the source list is empty, we need to ensure there's a newline separating it
        // from the rest
        return cmakelists.insert(insertion_point, '\n');
    }
    return cmakelists.erase(insertion_point, last_source_list_char.base());
}

auto insert_sources(std::string&                    cmakelists,
                    std::string::iterator           insertion_point,
                    std::vector<std::string> const& sources,
                    std::string const&              indent) {
    bool first_iteration = true;

    for (auto const& source : sources) {
        if (!first_iteration) {
            insertion_point = std::next(cmakelists.insert(insertion_point, '\n'));
        } else {
            first_iteration = false;
        }

        if (!source.empty()) {
            insertion_point
                = std::next(cmakelists.insert(insertion_point, indent.begin(), indent.end()),
                            indent.size());
        }
        insertion_point
            = std::next(cmakelists.insert(insertion_point, source.begin(), source.end()),
                        source.size());
    }

    return insertion_point;
}

std::pair<std::string::iterator, std::string::iterator>
write_sources(std::string&                    cmakelists,
              std::vector<std::string> const& source_strings,
              std::string::iterator           insertion_comment,
              std::string::iterator           begin_fn,
              std::string::iterator           end_fn) {
    std::string const indent
        = ::get_indent_at_indicator_comment(insertion_comment, begin_fn, end_fn);
    auto insertion_point = ::find_insertion_location(insertion_comment, begin_fn, end_fn);

    // Note: invalidates other iterators
    insertion_point = ::erase_existing_sources(cmakelists, insertion_point, begin_fn, end_fn);

    auto const end_insertion
        = ::insert_sources(cmakelists, insertion_point, source_strings, indent);
    return std::pair{end_insertion, cmakelists.end()};
}

}  // namespace

void pf::update_source_files(fs::path const&              cmakelists_file,
                             std::vector<fs::path> const& source_files,
                             pf::update_grouping          grouping) {
    if (!fs::exists(cmakelists_file)) {
        throw std::system_error{
            std::make_error_code(std::errc::no_such_file_or_directory),
            cmakelists_file.string() + " does not exist",
        };
    }

    std::string cmakelists = pf::slurp_file(cmakelists_file);

    std::vector<std::string> const source_strings
        = ::relative_source_strings(source_files, cmakelists_file.parent_path(), grouping);

    std::string const cmakelists_cpy = cmakelists;

    auto begin = cmakelists.begin();
    auto end   = cmakelists.end();

    while (begin != end) {
        auto const begin_fn          = ::find_next_function(begin, end);
        auto const end_fn            = ::find_end_function(begin_fn, end);
        auto const insertion_comment = ::find_insertion_indicator_comment(begin_fn, end_fn);

        if (insertion_comment == end_fn) {
            begin = end_fn;
            continue;
        }

        std::tie(begin, end)
            = ::write_sources(cmakelists, source_strings, insertion_comment, begin_fn, end_fn);
    }

    if (cmakelists != cmakelists_cpy) {
        std::fstream cmakelists_out = pf::open(cmakelists_file, std::ios::trunc | std::ios::out);
        cmakelists_out << cmakelists;
    }
}
