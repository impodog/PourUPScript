//
// Created by Dogs-Cute on 5/27/2023.
//
#include "pups.h"
#include "modules/std/std.h"
#include <cstring>
#include <iostream>
#include <filesystem>

#ifdef _WIN32
constexpr const char *test_folder = ".\\test\\";
#else
constexpr const char *test_folder = "./test/";
#endif
constexpr const char *desired_suffix = ".puppy";
constexpr const size_t suffix_len = 6;

int main(int argc, char **argv) {
    /* Example of running a simple program using PUPS
     * Each PUPS program is made up of two parts: Constants and PUPS code.
     * By using the simple class Constants, you may load desired constants.
     * Then, by using Control, you can run whole code file, or by line, or by ids.*/

    // Load necessary files(you can ignore this)
    using std::filesystem::path;
    std::string filename;
    std::cout << "Input your file to be executed(under " << test_folder << "):" << std::endl;
    std::cin >> filename;
    filename = test_folder + filename;
    if (filename.size() < suffix_len || filename.substr(filename.size() - suffix_len, suffix_len) != ".puppy")
        filename.append(desired_suffix);
    std::cout << "Compiling..." << std::endl;
    system(("python puppyscript/main.py " + filename).c_str());
    path filepath = filename;
    path const_path = filepath.parent_path().append(filepath.stem().string() + ".con");
    path pups_path = filepath.parent_path().append(filepath.stem().string() + ".pups");
    std::cout << "Constant Path = " << const_path << "; PUPS Path = " << pups_path << std::endl;
    std::cout << "Running..." << std::endl;

    //  Run the program
    pups::Constants constants(const_path); // Load necessary constants from the file
    pups::init(constants); // Init built-ins by adding them to constants
    pups::modules::pups_std::init(constants); // Init standard modules
    pups::Control control(pups_path, constants); // Creating the main file control by using the constants
    control.run(); // Run the whole file

    pups::quit(); // This is necessary for erasing global variables
    return 0;
}
