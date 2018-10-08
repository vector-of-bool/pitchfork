#include <pf/existing/glob_sources.hpp>

#include <algorithm>
#include <iterator>
#include <set>
#include <stdexcept>

#include <catch2/catch.hpp>

namespace fs = pf::fs;

namespace {
std::set<fs::path> make_relative(std::set<fs::path> const& paths) {
    static auto const base = fs::path{PF_TEST_BINDIR} / fs::path{"existing/sample/project"};

    std::set<fs::path> result;

    std::transform(paths.begin(),
                   paths.end(),
                   std::inserter(result, result.end()),
                   [](auto const& path) { return fs::relative(path, base); });

    return result;
}

}  // namespace

TEST_CASE("glob sources") {
    auto expected = std::set<fs::path>{
        fs::path{"src/project/subfolder/source1.c"},
        fs::path{"src/project/subfolder/source2.cc"},
        fs::path{"src/project/subfolder/source3.cpp"},
        fs::path{"src/project/subfolder/source4.cxx"},
        fs::path{"src/project/subfolder/source5.c++"},
        fs::path{"src/project/source1.c"},
        fs::path{"src/project/source2.cc"},
        fs::path{"src/project/source3.cpp"},
        fs::path{"src/project/source4.cxx"},
        fs::path{"src/project/source5.c++"},
    };

    CHECK(::make_relative(
              pf::glob_sources(fs::path{PF_TEST_BINDIR} / fs::path{"existing/sample/project"}))
          == expected);
}

TEST_CASE("glob headers") {
    auto expected = std::set<fs::path>{
        fs::path{"src/project/subfolder/header1.h"},
        fs::path{"src/project/subfolder/header2.hh"},
        fs::path{"src/project/subfolder/header3.hpp"},
        fs::path{"src/project/subfolder/header4.hxx"},
        fs::path{"src/project/subfolder/header5.h++"},
        fs::path{"src/project/header1.h"},
        fs::path{"src/project/header2.hh"},
        fs::path{"src/project/header3.hpp"},
        fs::path{"src/project/header4.hxx"},
        fs::path{"src/project/header5.h++"},
    };

    CHECK(::make_relative(
              pf::glob_headers(fs::path{PF_TEST_BINDIR} / fs::path{"existing/sample/project"}))
          == expected);
}
