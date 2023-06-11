//
// Created by Dogs-Cute on 5/27/2023.
//
#include "pups.h"

int main() {
    /* Example of running a simple program using PUPS
     * Each PUPS program is made up of two parts: Constants and PUPS code.
     * By using the simple class Constants, you may load desired constants.
     * Then, by using Control, you can run whole code file, or by line, or by ids.*/
    pups::Constants constants("test/test.con"); // Load necessary constants from the file
    pups::init_builtins(constants); // Init built-ins by adding them to constants
    pups::Control control("test/test.pups", constants); // Creating the main file control by using the constants
    control.run(); // Run the whole file
    return 0;
}