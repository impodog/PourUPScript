#include <iostream>
#include "argparse.hpp"
#include "const.h"
#include "processor.h"
#include "export.h"

int main(int argc, char **argv) {
    argparse::ArgumentParser program("cpp-puppy", PUPPY_VERSION);
    program.add_argument("file")
            .help("the puppyscript file to be compiled");
    program.add_argument("-o", "--output")
            .help("the output file, ending with \".pups\" automatically");
    program.add_argument("--keep-temp")
            .help("whether to keep temporary files")
            .nargs(0)
            .implicit_value(true)
            .default_value(false);
    program.add_argument("--debug")
            .help("whether to generate debug info lines")
            .nargs(0)
            .implicit_value(true)
            .default_value(false);
    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 0;
    }

    auto file = std::filesystem::absolute(program.get<std::string>("file"));
    path output = file.parent_path() / (file.stem().string() + ".pups");
    if (auto output_present = program.present("--output")) {
        output = *output_present;
        if (output.extension() != ".pups") {
            output = output.string() + ".pups";
        }
    }
    auto keep_temp = program.get<bool>("--keep-temp");
    auto debug = program.get<bool>("--debug");

    script(file, output, keep_temp, debug);
    return 0;
}
