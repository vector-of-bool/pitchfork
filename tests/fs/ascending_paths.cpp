#include <pf/fs/ascending_paths.hpp>

#include <range/v3/algorithm/reverse.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/drop_while.hpp>
#include <range/v3/view/partial_sum.hpp>

#include <catch2/catch.hpp>

namespace fs = std::filesystem;

TEST_CASE("ascending_paths makes sense") {
    fs::path p = GENERATE(values({
        fs::path{""},
        fs::current_path(),
        fs::path{"/some/long/path/that/probably/doesnt/exist"},
    }));
    CAPTURE(p);

    std::vector<fs::path> const result = pf::ascending_paths(p) | ranges::to_vector;

    std::vector<fs::path> expected = fs::weakly_canonical(p)
        | ranges::view::partial_sum([](fs::path const& lhs, fs::path const& rhs) {
                                         return lhs / rhs;
                                     })
        | ranges::view::drop_while([](fs::path const& p) { return !p.has_relative_path(); })
        | ranges::to_vector;
    ranges::reverse(expected);

    CHECK(result == expected);
}
