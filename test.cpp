//
// Created by Dogs-Cute on 5/27/2023.
//
#include "pups.h"
#include <iostream>
#include <filesystem>

int main() {
    /* Example of running a simple program using PUPS
     * Each PUPS program is made up of two parts: Constants and PUPS code.
     * By using the simple class Constants, you may load desired constants.
     * Then, by using Control, you can run whole code file, or by line, or by ids.*/

    // Load necessary files(you can ignore this)
    using std::filesystem::path;
    std::string filename;
    std::cout << "Input your file to be executed:" << std::endl;
    std::cin >> filename;
    std::cout << "Compiling..." << std::endl;
    system(("python puppyscript/main.py " + filename).c_str());
    std::cout << "Running..." << std::endl;
    path filepath = filename;
    path const_path = filepath.parent_path().append(filepath.stem().string() + ".con");
    path pups_path = filepath.parent_path().append(filepath.stem().string() + ".pups");
    std::cout << "Constant Path = " << const_path << "; PUPS Path = " << pups_path << std::endl;

    //  Run the program
    pups::Constants constants(const_path); // Load necessary constants from the file
    pups::init_builtins(constants); // Init built-ins by adding them to constants
    pups::Control control(pups_path, constants); // Creating the main file control by using the constants
    control.run(); // Run the whole file
    return 0;
}