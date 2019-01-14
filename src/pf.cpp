#include <args.hxx>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <pf/existing.hpp>
#include <pf/fs.hpp>
#include <pf/new.hpp>
#include <pf/pitchfork.hpp>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <unordered_map>

namespace fs = pf::fs;

namespace {

using string_flag = args::ValueFlag<std::string>;
using path_flag   = args::ValueFlag<fs::path>;

class reached_eof : public std::exception {};

std::string get_input_line() {
    std::cout.flush();
    std::cerr.flush();
    std::string ret;
    std::getline(std::cin, ret);
    if (std::cin.eof()) {
        throw reached_eof();
    }
    return ret;
}

fs::path default_base_dir() {
    auto ptr = std::getenv("PF_BASE_DIR");
    if (ptr) {
        return ptr;
    }

    // Attempt to detect the base directory
    return pf::detect_base_dir()
        // Just use the cwd if cannot detect base dir
        .value_or(fs::current_path());
}

struct cli_common {
    args::ArgumentParser&           parser;
    std::shared_ptr<spdlog::logger> console = spdlog::stdout_color_mt("console");
    // Flags that are not subcommand-specific:
    args::HelpFlag help{parser, "help", "Print this help message", {'h', "help"}};
    // base-dir determines where projects will live
    path_flag base_dir_arg{parser,
                           "base_dir",
                           "The base directory for projects\n[env: PF_BASE_DIR]",
                           {'B', "base-dir"},
                           default_base_dir()};

    args::Group cmd_group{parser, "Available Commands"};

    explicit cli_common(args::ArgumentParser& args)
        : parser{args} {}

    fs::path get_base_dir() {
        auto base_dir = base_dir_arg.Get();
        if (!base_dir.empty()) {
            return fs::absolute(base_dir);
        }
        // Just use the cwd if the base dir provided was empty
        return fs::current_path();
    }
};

class cmd_list {
private:
    cli_common&   _cli;
    args::Command _cmd{_cli.cmd_group, "list", "List projects"};

public:
    explicit cmd_list(cli_common& gl)
        : _cli{gl} {}

    explicit operator bool() const { return !!_cmd; }

    int run() {
        std::error_code ec;
        const auto      base_dir = _cli.get_base_dir();
        auto            iter     = fs::directory_iterator{base_dir, ec};
        if (ec) {
            _cli.console->error("Failed to enumerate directory ({}): {}", base_dir, ec.message());
            return 1;
        }

        for (fs::path child : fs::directory_iterator{base_dir}) {
            if (!fs::is_directory(child, ec)) {
                if (ec) {
                    _cli.console->warn("Failed to enumerate item ({}): {}", child, ec.message());
                }
                continue;
            }
            std::cout << child.filename().string() << '\n';
        }
        return 0;
    }
};

class toggle_flag : public args::Flag {
public:
    enum state {
        unset,
        enabled,
        disabled,
    };
    state _state = unset;

private:
    std::string GetNameString(const args::HelpParams&) const override { return "--[no-]" + Name(); }

    args::FlagBase* Match(const args::EitherFlag& flag) override {
        auto ret = Flag::Match(flag);
        if (ret) {
            auto off = flag.longFlag.find("no-");
            _state   = (off == 0) ? disabled : enabled;
        }
        return ret;
    }

public:
    toggle_flag(args::Group& grp, const std::string& name, const std::string& help)
        : Flag::Flag(grp, name, help, {"no-" + name, name}) {}

    enum state state() const noexcept { return _state; }
};

template <typename FlagType>
FlagType get_string_value(args::ValueFlag<FlagType>& flag, const std::string& message) {
    auto ret = flag.Get();
    while (ret.empty()) {
        std::cout << message << ": ";
        ret = FlagType(get_input_line());
    }
    return ret;
}

template <typename FlagType>
FlagType get_string_value(args::ValueFlag<FlagType>& flag,
                          const std::string&         message,
                          const FlagType&            default_) {
    auto ret = flag.Get();
    if (!ret.empty()) {
        return ret;
    }

    std::cout << message << " [" << default_ << "]: ";
    ret = FlagType(get_input_line());
    if (ret.empty()) {
        return default_;
    }
    return ret;
}

bool get_toggle_value(const toggle_flag& flag, const std::string& message, bool default_) {
    if (flag.state() == flag.enabled) {
        return true;
    } else if (flag.state() == flag.disabled) {
        return false;
    }

    while (1) {
        std::cout << message;
        std::cout << (default_ ? " [Yn]: " : " [yN]: ");
        auto chosen = get_input_line();
        if (chosen.empty()) {
            return default_;
        }
        auto c = chosen[0];
        if (c == 'y' || c == 'Y') {
            return true;
        } else if (c == 'n' || c == 'N') {
            return false;
        }
    }
}

template <typename Map>
typename Map::value_type::second_type
get_map_value(const std::string& message, const Map& map, const std::string& default_ = "") {
    while (1) {
        std::cout << message << '\n';
        std::cout << "Chose one of:\n";
        for (auto& pair : map) {
            std::cout << "  - " << pair.first << '\n';
        }
        if (default_.empty()) {
            std::cout << "Selection: ";
        } else {
            std::cout << fmt::format("Selection [{}]: ", default_);
        }
        std::cout.flush();
        auto chosen = get_input_line();
        if (chosen.empty() && !default_.empty()) {
            chosen = default_;
        }
        auto found = map.find(chosen);
        if (found != map.end()) {
            return found->second;
        }
    }
}

class cmd_new {
private:
    cli_common&    _cli;
    args::Command  _cmd{_cli.cmd_group, "new", "Create a new project"};
    args::HelpFlag _help{_cmd, "help", "Print help for the `new` subcommand", {'h', "help"}};
    string_flag    _name{_cmd, "name", "Name for the new project", {"name"}};
    string_flag _namespace{_cmd, "namespace", "The root namespace for the project", {"namespace"}};
    toggle_flag _split_headers{_cmd, "split-headers", "Store headers separate from source files"};
    toggle_flag _gen_tests{_cmd, "tests", "Generate a tests/ directory"};
    toggle_flag _gen_third_party{_cmd, "third-party", "Generate a third_party/ directory"};
    toggle_flag _gen_examples{_cmd, "examples", "Generate an examples/ directory"};
    toggle_flag _gen_extras{_cmd, "extras", "Generate an extras/ directory"};
    string_flag
        _first_file_stem{_cmd,
                         "first-file",
                         "Stem of the first file to create in the root namespace (No extension)",
                         {"first-file"}};

    std::unordered_map<std::string, pf::build_system> _bs_map{
        {"none", pf::build_system::none},
        {"cmake", pf::build_system::cmake},
    };
    args::MapFlag<std::string, pf::build_system> _build_system{_cmd,
                                                               "build-system",
                                                               "The build system to generate",
                                                               {'b', "build-system"},
                                                               _bs_map};

public:
    explicit cmd_new(cli_common& gl)
        : _cli{gl} {}

    explicit operator bool() const { return !!_cmd; }

    int run() {

        // Get the project name
        auto pr_name = get_string_value(_name, "Name for the new project");

        // Check on the directory which we will create
        auto            new_pr_dir = fs::absolute(_cli.get_base_dir() / pr_name);
        std::error_code ec;
        if (fs::exists(new_pr_dir, ec)) {
            _cli.console->error(
                "Cannot create project: Destination path names an existing file or directory ({})",
                fs::canonical(new_pr_dir));
            return 1;
        }
        if (ec) {
            _cli.console->error("Failed to check on directory ({}): {}", new_pr_dir, ec.message());
            return 2;
        }

        // Fill out the parameters for the new project
        auto root_namespace
            = get_string_value(_namespace, "Initial namespace", pf::namespace_for_name(pr_name));

        // Final stuff
        auto first_file_stem = get_string_value(_first_file_stem,
                                                "First file stem (No extension)",
                                                new_pr_dir.stem().string());

        // We have enough for the initial params set
        pf::new_project_params params{pr_name, root_namespace, first_file_stem, new_pr_dir};

        // Process toggles
        params.separate_headers
            = get_toggle_value(_split_headers, "Split headers and sources?", false);
        params.create_tests = get_toggle_value(_gen_tests, "Generate a tests/ directory?", true);
        params.create_third_party
            = get_toggle_value(_gen_third_party, "Generate a third_party/ directory?", true);
        params.create_examples
            = get_toggle_value(_gen_examples, "Generate an examples/ directory?", true);
        params.create_extras
            = get_toggle_value(_gen_extras, "Generate an extras/ directory?", false);

        auto bs = _build_system.Get();
        if (bs == pf::build_system::unspecified) {
            bs = get_map_value("Build system to generate", _bs_map, "cmake");
        }
        params.build_system = bs;

        // Create the project!
        try {
            pf::create_project(params);
        } catch (const std::system_error& e) {
            _cli.console->error("Failed to create project in {}: {}", new_pr_dir, ec.message());
            return 1;
        }
        return 0;
    }
};

class cmd_update {
private:
    cli_common&    _cli;
    args::Command  _cmd{_cli.cmd_group, "update", "Update sources for existing project"};
    args::HelpFlag _help{_cmd, "help", "Print help for the `update` subcommand", {'h', "help"}};
    std::unordered_map<std::string, pf::build_system> _bs_map{
        {"cmake", pf::build_system::cmake},
    };
    args::MapFlag<std::string, pf::build_system> _build_system{_cmd,
                                                               "build-system",
                                                               "The build system to update",
                                                               {'b', "build-system"},
                                                               _bs_map};

public:
    explicit cmd_update(cli_common& gl)
        : _cli{gl} {}

    explicit operator bool() const { return !!_cmd; }

    int run() {
        auto bs = _build_system.Get();
        if (bs == pf::build_system::unspecified) {
            bs = get_map_value("Build system whose files to update", _bs_map, "cmake");
        }

        // Only CMake supported at this time
        if (bs != pf::build_system::cmake) {
            _cli.console->error(
                "CMake is the only supported build system for the `update` subcommand");
            return 1;
        }

        // Update existing source files
        try {
            auto const base_dir = _cli.get_base_dir();

            auto const            src_dir = base_dir / "src";
            std::vector<fs::path> sources = pf::glob_sources(src_dir);
            pf::update_source_files(src_dir / "CMakeLists.txt", sources);

            auto const tests_dir = base_dir / "tests";
            if (fs::exists(tests_dir)) {
                std::vector<fs::path> test_sources = pf::glob_sources(tests_dir);
                pf::update_source_files(tests_dir / "CMakeLists.txt", test_sources);
            }
        } catch (const std::system_error& e) {
            _cli.console->error("Failed to update project in {}: {}",
                                _cli.get_base_dir(),
                                e.what());
            return 1;
        }
        return 0;
    }
};

class cmd_query {
private:
    cli_common&    _cli;
    args::Command  _cmd{_cli.cmd_group, "query", "Query the project"};
    args::HelpFlag _help{_cmd, "help", "Print help for the `query` subcommand", {'h', "help"}};
    args::Positional<std::string> _id{
        _cmd,
        "id",
        "Obtain this information. Valid values:\n"
        "* project.root",
    };

public:
    explicit cmd_query(cli_common& gl)
        : _cli{gl} {}

    explicit operator bool() const { return !!_cmd; }

    int run() {
        static std::unordered_map<std::string, std::function<int(cmd_query const&)>> queries{
            {"project.root",
             [](cmd_query const& cmd) {
                 std::cout << cmd._cli.get_base_dir().string() << '\n';
                 return 0;
             }},
        };

        auto const id = _id.Get();

        auto query = queries.find(id);
        if (query == queries.end()) {
            _cli.console->error(
                "Invalid id `{}`. Valid values:\n"
                "* project.root",
                id);
            return 1;
        }

        return query->second(*this);
    }
};

}  // namespace

int main(int argc, char** argv) {
    args::ArgumentParser parser("Pitchfork: It's for your project.");
    cli_common           args{parser};

    // Subcommands
    cmd_list   list{args};
    cmd_new    new_{args};
    cmd_update update{args};
    cmd_query  query{args};

    try {
        parser.ParseCLI(argc, argv);
    } catch (args::Help const&) {
        std::cout << parser;
        return 0;
    } catch (args::Error& e) {
        std::cerr << e.what() << '\n' << parser;
        return 1;
    }

    try {
        if (list) {
            return list.run();
        } else if (new_) {
            return new_.run();
        } else if (update) {
            return update.run();
        } else if (query) {
            return query.run();
        } else {
            assert(false && "No subcommand selected?");
            std::terminate();
        }
    } catch (const reached_eof&) {
        return 2;
    }
}
