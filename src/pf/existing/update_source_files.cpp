#include "./update_source_files.hpp"

#include <cctype>
#include <fstream>
#include <sstream>

#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

namespace fs = pf::fs;

void pf::update_source_files(fs::path const& project_root, std::set<fs::path> const& source_files) {
    fs::path const src_dir        = project_root / "src";
    fs::path const src_cmakelists = src_dir / "CMakeLists.txt";

    if (fs::exists(src_cmakelists)) {
        std::vector<std::string> source_strings;
        source_strings.reserve(source_files.size());
        std::transform(source_files.begin(),
                       source_files.end(),
                       std::back_inserter(source_strings),
                       [&src_dir](auto const& path) {
                           auto result = fs::relative(path, src_dir).string();
                           // TODO: evaluate if replacing `\` in filenames might cause a problem
                           std::replace(result.begin(), result.end(), '\\', '/');

                           return result;
                       });

        std::ifstream cmakelists_file{src_cmakelists};
        std::string   cmakelists = [&] {
            std::ostringstream out;
            out << cmakelists_file.rdbuf();
            return out.str();
        }();

        std::string cmakelists_cpy = cmakelists;

        auto begin = cmakelists.begin();
        auto end   = cmakelists.end();

        while (begin != end) {
            // BUG: fails for ( in a comment.
            // TODO: use a proper CMake parser, or otherwise improve this
            begin = std::find(begin, end, '(');
            if (begin == end) {
                break;
            }

            auto end_fn = std::find(begin, end, ')');

            static constexpr std::string_view SourcesComment = "# sources\n";

            auto const insertion_comment
                = std::search(begin, end_fn, SourcesComment.begin(), SourcesComment.end());

            if (insertion_comment == end_fn) {
                begin = end_fn;
                continue;
            }

            auto const begin_of_line
                = std::find_if(std::make_reverse_iterator(insertion_comment),
                               std::make_reverse_iterator(begin),
                               [](char c) { return c == '\n' || !std::isspace(c); })
                      .base();

            auto indent_begin = begin_of_line;
            auto indent_end   = insertion_comment;

            auto const insertion_point = std::next(insertion_comment, SourcesComment.size());

            auto const begin_loc        = std::distance(cmakelists.begin(), begin);
            auto       insertion_loc    = std::distance(cmakelists.begin(), insertion_point);
            auto       indent_begin_loc = std::distance(cmakelists.begin(), indent_begin);
            auto       indent_end_loc   = std::distance(cmakelists.begin(), indent_end);

            auto last_file_char
                = std::next(std::find_if(std::make_reverse_iterator(end_fn),
                                         std::make_reverse_iterator(insertion_point),
                                         [](char c) { return c == '\n' || !std::isspace(c); }))
                      .base();

            cmakelists.erase(insertion_point, last_file_char);
            // All iterators invalidated

            bool first_iteration = true;
            for (auto const& file : source_strings) {
                if (!first_iteration) {
                    cmakelists.insert(insertion_loc, 1, '\n');
                    insertion_loc += 1;
                } else {
                    first_iteration = false;
                }

                cmakelists.insert(insertion_loc,
                                  std::string_view{cmakelists}.substr(indent_begin_loc,
                                                                      indent_end_loc
                                                                          - indent_begin_loc));
                insertion_loc += indent_end_loc - indent_begin_loc;

                cmakelists.insert(insertion_loc, file);
                insertion_loc += file.size();
            }

            begin = cmakelists.begin() + insertion_loc;
            end   = cmakelists.end();
        }

        std::ofstream cmakelists_out{src_cmakelists, std::ios::trunc};
        cmakelists_out << cmakelists;
    } else {
        spdlog::get("console")->warn("No CMakeLists.txt found at: {}", src_cmakelists);
    }
}
