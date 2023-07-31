#include <iostream>
#include "argparse.hpp"
#include "const.h"
#include "processor.h"

void script(path file, const path &output, bool keep_temp, bool debug) {
    path tmp = file;
    std::string output_name = (output.parent_path() / output.stem().string()).string();
    const auto move_on = [&tmp, &file, keep_temp]() {
        if (!keep_temp) {
            std::filesystem::remove(tmp);
        }
        tmp = file;
    };
    globals = std::make_unique<Globals>(file);
    tmp = file = preprocess(file, debug, output_name);

    file = extract(file, debug, output_name);
    move_on();

    file = mid_process(file, debug, output_name);
    move_on();

    file = macro_rules(file, debug, output_name);
    move_on();

    file = syntactic(file, debug, output_name);
    move_on();

    file = type_format(file, debug, output_name);
    move_on();

    file = structure(file, debug, output_name);
    move_on();

    file = assignment(file, debug, output_name);
    move_on();

    file = brackets(file, debug, output_name);
    move_on();

    file = expr_optim(file, debug, output_name);
    move_on();

    file = shortcuts(file, debug, output_name);
    move_on();
}

int main(int argc, char **argv) {
    argparse::ArgumentParser program("cpp-puppy");
    program.add_argument("file")
            .help("the file to be compiled");
    program.add_argument("-o", "--output")
            .help("output file(.pups)");
    program.add_argument("--keep-temp")
            .help("keep temporary files")
            .nargs(0)
            .implicit_value(true)
            .default_value(false);
    program.add_argument("--debug")
            .help("turn on debug mode")
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
