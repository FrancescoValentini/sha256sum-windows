#include <iostream>
#include <argparse/argparse.hpp>
#include "sha256sum-windows.h"

static void setupArguments(argparse::ArgumentParser& program) {
    // FILE (positional)
    program.add_argument("files")
        .help("Files to process, or '-' for standard input")
        .default_value(std::vector<std::string>{})
        .nargs(argparse::nargs_pattern::any);

    // Checksum verification mode
    program.add_argument("-c", "--check")
        .help("read checksums from the FILEs and check them")
        .default_value(false)
        .implicit_value(true);

    // Checksum verification options
    program.add_argument("--ignore-missing")
        .help("don't fail or report status for missing files")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("--quiet")
        .help("don't print OK for each successfully verified file")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("--status")
        .help("don't output anything, status code shows success")
        .default_value(false)
        .implicit_value(true);
}

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program(PGM_NAME);

    setupArguments(program);

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    bool check = program.get<bool>("--check");
    std::vector<std::string> files = program.get<std::vector<std::string>>("files");

    std::cout << "check: " << check << "\n";
    std::cout << "files:\n";
    for (const auto& f : files) {
        std::cout << "- " << f << "\n";
    }

    return EXIT_OK;
}
