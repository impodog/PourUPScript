//
// Created by Dogs-Cute on 5/28/2023.
//

#include "keyword_func.h"

namespace pups::library::builtins::keyword_func {

    Print::Print() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        while (!args.empty()) {
            std::cout << args.front()->get()->str();
            args.pop();
        }
        std::cout << std::endl;
        return pending;
    }) {}

    MoveTo::MoveTo() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        while (!args.empty()) {
            *args.front() = map->get_temp();
            args.pop();
        }
        return pending;
    }) {}

    Id id_print{"", "print"}, id_moveTo{"", "mov"};

    void init(Constants &constants) {
        constants.add(id_print, std::make_shared<Print>());
        constants.add(id_moveTo, std::make_shared<MoveTo>());
    }
}