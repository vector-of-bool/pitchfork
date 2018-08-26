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

        // Get the new root namespace for the project.
        auto default_ns       = pf::namespace_for_name(params.name);
        params.root_namespace = get_namespace(default_ns);

        // Create the project!
        pf::create_directories(params, ec);
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