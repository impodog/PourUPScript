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

    Id id_inputs{"", "inputs"};

    void init(Constants &constants) {
        constants.add(id_inputs, std::make_shared<Inputs>());
    }
}