//
// Created by impodog on 6/4/2023.
//

#include "builtin_func.h"
#include "strings.h"
#include <iostream>

namespace pups::library::builtins::builtin_func {

    Inputs::Inputs() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        while (!args.empty()) {
            std::string buf;
            std::cin >> buf;
            *args.front() = std::make_shared<strings::String>(buf);
            args.pop();
        }
        return pending;
    }) {}


    Println::Println() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        while (!args.empty()) {
            std::cout << args.front()->get()->str();
            args.pop();
        }
        std::cout << std::endl;
        return pending;
    }) {}

    Print::Print() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        while (!args.empty()) {
            std::cout << args.front()->get()->str();
            args.pop();
        }
        return pending;
    }) {}

    Id id_inputs{"", "inputs"}, id_println{"", "println"}, id_print{"", "print"};

    void init(Constants &constants) {
        constants.add(id_inputs, std::make_shared<Inputs>());
        constants.add(id_println, std::make_shared<Println>());
        constants.add(id_print, std::make_shared<Print>());
    }
}