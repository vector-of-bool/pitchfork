#include <pf/existing/update_source_files.hpp>

#include <fstream>
#include <stdexcept>
#include <string>

#include <boost/optional/optional_io.hpp>
#include <boost/scope_exit.hpp>

#include <catch2/catch.hpp>

namespace fs = pf::fs;

TEST_CASE("update source files") {
    BOOST_SCOPE_EXIT(void) {
        if (!fs::copy_file(fs::path{PF_TEST_SRCDIR "/existing/sample/project/src/CMakeLists.txt"},
                           fs::path{PF_TEST_BINDIR "/existing/sample/project/src/CMakeLists.txt"},
                           fs::copy_options::overwrite_existing)) {
            throw std::runtime_error{
                "Critical failure: unable to restore test state. Please rerun cmake"};
        }
    }
    BOOST_SCOPE_EXIT_END

    INFO("file (compare with '<file>.after_update'): " << PF_TEST_BINDIR
         "/existing/sample/project/src/CMakeLists.txt");

    pf::update_source_files(
        fs::path{PF_TEST_BINDIR "/existing/sample/project"},
        {
            fs::path{PF_TEST_BINDIR "/existing/sample/project/src/project/header1.h"},
            fs::path{PF_TEST_BINDIR "/existing/sample/project/src/project/header2.hh"},
            fs::path{PF_TEST_BINDIR "/existing/sample/project/src/project/header3.hpp"},
            fs::path{PF_TEST_BINDIR "/existing/sample/project/src/project/header4.hxx"},
            fs::path{PF_TEST_BINDIR "/existing/sample/project/src/project/header5.h++"},
            fs::path{PF_TEST_BINDIR "/existing/sample/project/src/project/subfolder/header1.h"},
            fs::path{PF_TEST_BINDIR "/existing/sample/project/src/project/subfolder/header2.hh"},
            fs::path{PF_TEST_BINDIR "/existing/sample/project/src/project/subfolder/header3.hpp"},
            fs::path{PF_TEST_BINDIR "/existing/sample/project/src/project/subfolder/header4.hxx"},
            fs::path{PF_TEST_BINDIR "/existing/sample/project/src/project/subfolder/header5.h++"},
        },
        {
            fs::path{PF_TEST_BINDIR "/existing/sample/project/src/project/source1.c"},
            fs::path{PF_TEST_BINDIR "/existing/sample/project/src/project/source2.cc"},
            fs::path{PF_TEST_BINDIR "/existing/sample/project/src/project/source3.cpp"},
            fs::path{PF_TEST_BINDIR "/existing/sample/project/src/project/source4.cxx"},
            fs::path{PF_TEST_BINDIR "/existing/sample/project/src/project/source5.c++"},
            fs::path{PF_TEST_BINDIR "/existing/sample/project/src/project/subfolder/source1.c"},
            fs::path{PF_TEST_BINDIR "/existing/sample/project/src/project/subfolder/source2.cc"},
            fs::path{PF_TEST_BINDIR "/existing/sample/project/src/project/subfolder/source3.cpp"},
            fs::path{PF_TEST_BINDIR "/existing/sample/project/src/project/subfolder/source4.cxx"},
            fs::path{PF_TEST_BINDIR "/existing/sample/project/src/project/subfolder/source5.c++"},
        });

    std::ifstream expected_file{
        fs::path{PF_TEST_BINDIR "/existing/sample/project/src/CMakeLists.txt.after_update"},
    };

    std::ifstream actual_file{
        fs::path{PF_TEST_BINDIR "/existing/sample/project/src/CMakeLists.txt"},
    };

    std::string expected_line, actual_line;

    int line = 0;
    while (std::getline(actual_file, actual_line) && std::getline(expected_file, expected_line)) {
        CAPTURE(line);
        // Don't use CHECK(...) because then a single line difference could output a lot of lines
        REQUIRE(actual_line == expected_line);
        ++line;
    }

    INFO("Files differ after line: " << line);
    CHECK(static_cast<bool>(actual_file) == static_cast<bool>(expected_file));
}
