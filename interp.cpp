//
// Created by Dogs-Cute on 4/29/2023.
//

#include "pups.h"
#include "std_lib/include.hpp"
#include <iostream>

#define str_eq(s1, s2) (!strcmp(s1, s2))


int main(int argc, char **argv) {
    char *file = nullptr;
    for (int i = 1; i < argc; i++) {
        char *str = argv[i];
        switch (str[0]) {
            case '-':
                switch (str[1]) {
                    case '-':
                        if (str_eq(str, "--keep-temporary"))
                            PUPS::keepTemporary = true;
                        else if (str_eq(str, "--version"))
                            std::cout << "PourUPScript" << PUPS::version << std::endl;
                        else if (str_eq(str, "--include-std"))
                            PUPS::INCLUDE_ALL();
                        else
                            std::cout << "WARNING : Unknown option \"" << str << "\"\n";
                        break;
                    default:
                        std::cout << "WARNING : Unknown option \"" << str << "\"\n";
                        break;
                }
                break;
            default:
                if (file)
                    std::cout << "WARNING : Multiple file arguments given.\n";
                file = str;
                break;
        }
    }
    if (!file) {
        std::cout << "ERROR : No file given. Abort.";
        exit(1);
    }
    PUPS::Simple::init();
    PUPS::Simple::InterpFile load{std::string(file)};
    load.run();
    return 0;
}
