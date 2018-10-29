#include <pf/existing/detect_base_dir.hpp>

#include <catch2/catch.hpp>

namespace fs = pf::fs;

TEST_CASE("detect project root") {
    auto const cases = {
        fs::path{"existing/sample/project"},
        fs::path{"existing/sample/project/data"},
        fs::path{"existing/sample/project/build"},
        fs::path{"existing/sample/parallel_build"},

        // This is a harder case, since it actually does contain a CMakeLists.txt
        fs::path{"existing/sample/project/src"},
    };

    fs::path const expected = fs::path{PF_TEST_BINDIR} / fs::path{"existing/sample/project"};

    for (auto const& working_dir : cases) {
        DYNAMIC_SECTION(working_dir) {
            CHECK(pf::detect_base_dir(fs::path{PF_TEST_BINDIR} / working_dir) == expected);
        }
    }
}

TEST_CASE("no project root") {
    auto path = fs::current_path().root_directory();
    INFO("Base directory:");
    INFO(path);
    auto basedir = pf::detect_base_dir(path);
    CHECK(basedir == std::nullopt);
}
