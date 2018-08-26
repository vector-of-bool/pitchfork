#include <args.hxx>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <pf/fs.hpp>
#include <pf/new.hpp>
#include <pf/pitchfork.hpp>

#include <cassert>
#include <iostream>

namespace fs = pf::fs;

namespace {

using string_flag = args::ValueFlag<std::string>;
using path_flag   = args::ValueFlag<fs::path>;

class eof : public std::exception {};

std::string get_input_line() {
    std::cout.flush();
    std::cerr.flush();
    std::string ret;
    std::getline(std::cin, ret);
    if (std::cin.eof()) {
        throw eof();
    }
    return ret;
}

fs::path base_dir_env() {
    auto ptr = std::getenv("PF_BASE_DIR");
    return ptr ? ptr : fs::current_path();
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
                           base_dir_env()};

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

class cmd_new {
private:
    cli_common&    _cli;
    args::Command  _cmd{_cli.cmd_group, "new", "Create a new project"};
    args::HelpFlag _help{_cmd, "help", "Print help for the `new` subcommand", {'h', "help"}};
    string_flag    _name{_cmd, "name", "Name for the new project", {"name"}};
    string_flag _namespace{_cmd, "namespace", "The root namespace for the project", {"namespace"}};
    args::Group _split_headers_grp{_cmd, "Split headers?", args::Group::Validators::AtMostOne};
    args::Flag  _split_headers{_split_headers_grp,
                              "split-headers",
                              "Split headers in `include/` and sources in `src/",
                              {"split-headers"}};
    args::Flag  _no_split_headers{_split_headers_grp,
                                 "no-split-headers",
                                 "Keep headers and sources in the `src/` directory",
                                 {"no-split-headers"}};
    string_flag
        _first_file_stem{_cmd,
                         "first-file",
                         "Stem of the first file to create in the root namespace (No extension)",
                         {"first-file"}};

    std::string get_name() {
        std::string name = _name.Get();
        while (name.empty()) {
            std::cout << "New project name: ";
            name = get_input_line();
        }
        return name;
    }

    std::string get_namespace(const std::string& def_ns) {
        std::string ns = _namespace.Get();
        if (ns.empty()) {
            std::cout << fmt::format("New project root namespace [{}]: ", def_ns);
            ns = get_input_line();
            if (ns.empty()) {
                // They've opted to select the default project-name-as-namespace
                return def_ns;
            }
        }
        return ns;
    }

    bool get_should_split_headers() {
        if (_split_headers) {
            return true;
        }
        if (_no_split_headers) {
            return false;
        }
        while (1) {
            std::cout << "Separate headers and sources? [yN]: ";
            auto chosen = get_input_line();
            if (chosen.empty()) {
                return false;
            }
            auto c = chosen[0];
            if (c == 'y' || c == 'Y') {
                return true;
            } else if (c == 'n' || c == 'N') {
                return false;
            }
        }
    }

    std::string get_first_file_stem(const fs::path& dir) {
        auto ret = _first_file_stem.Get();
        if (!ret.empty()) {
            return ret;
        }
        auto default_fname = dir.stem().string();
        std::cout << fmt::format("First file stem (No extension): [{}]", default_fname);
        ret = get_input_line();
        if (ret.empty()) {
            return default_fname;
        }
        return ret;
    }

public:
    explicit cmd_new(cli_common& gl)
        : _cli{gl} {}

    explicit operator bool() const { return !!_cmd; }

    int run() {
        pf::new_project_params params;

        // Get the project name
        params.name = get_name();

        // Check on the directory which we will create
        auto            new_pr_dir = fs::absolute(_cli.get_base_dir() / params.name);
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
        params.directory = new_pr_dir;

        // Fill out the parameters for the new project
        params.root_namespace   = get_namespace(pf::namespace_for_name(params.name));
        params.separate_headers = get_should_split_headers();
        params.first_file_stem  = get_first_file_stem(params.directory);

        // Create the project!
        pf::create_project(params, ec);
        if (ec) {
            _cli.console->error("Failed to prepare directories for new project ({}): {}",
                                new_pr_dir,
                                ec.message());
            return 1;
        }
        return 1;
    }
};

}  // namespace

int main(int argc, char** argv) {
    args::ArgumentParser parser("Pitchfork: It's for your project.");
    cli_common           args{parser};

    // Subcommands
    cmd_list list{args};
    cmd_new  new_{args};

    try {
        parser.ParseCLI(argc, argv);
    } catch (args::Help) {
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
        } else {
            assert(false && "No subcommand selected?");
            std::terminate();
        }
    } catch (const eof&) {
        return 2;
    }
}