//
// Created by impodog on 6/4/2023.
//

#include "builtin_func.h"
#include "../types/strings.h"
#include <iostream>

namespace pups::library::builtins::builtin_func {

    Inputs::Inputs() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        while (!args.empty()) {
            std::string buf;
            std::cin >> buf;
            *args.front() = std::make_shared<strings::String>(buf);
            args.pop_front();
        }
        return pending;
    }) {}


    Print::Print() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        while (!args.empty()) {
            std::cout << args.front()->get()->str();
            args.pop_front();
        }
        std::cout << std::endl;
        return pending;
    }) {}

    Puts::Puts() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        while (!args.empty()) {
            std::cout << args.front()->get()->str();
            args.pop_front();
        }
        return pending;
    }) {}

    Call::Call() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        if (args.size() != 1)
            map->throw_error(std::make_shared<ArgumentError>("Call function requires one only argument."));
        else {
            auto ptr = cast<Function>(*args.front());
            if (ptr)
                return ptr->end_of_line(map);
            else
                map->throw_error(std::make_shared<TypeError>("Call function requires a Function."));
        }
        return pending;
    }) {}

    IsDefined::IsDefined() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        if (args.empty()) {
            map->throw_error(std::make_shared<ArgumentError>("is_def function requires at least one argument"));
            return numbers::False;
        }
        while (!args.empty()) {
            if (is_pending(*args.front()))
                return numbers::False;
            args.pop_front();
        }
        return numbers::True;
    }) {}

    TypenameCheck::TypenameCheck() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        if (args.size() != 2)
            map->throw_error(std::make_shared<ArgumentError>("TypenameCheck requires two only arguments."));
        else {
            const auto &first = *args.front();
            args.pop_front();
            auto name = cast<strings::String>(*args.front());
            args.pop_front();
            if (name)
                return first->type_name() == name->data() ? numbers::True : numbers::False;
            else
                map->throw_error(std::make_shared<TypeError>("TypenameCheck requires a string as the second argument."));
        }
        return numbers::False;
    }) {}

    Equal::Equal() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        if (args.size() != 2)
            map->throw_error(std::make_shared<ArgumentError>("Equal check requires two only arguments."));
        else {
            auto &lhs = *args.front();
            args.pop_front();
            auto &rhs = *args.front();
            args.pop_front();
            return lhs->equal(rhs) ? numbers::True : numbers::False;
        }
        return numbers::False;
    }) {}

    Id id_inputs{"", "inputs"}, id_print{"", "print"}, id_puts{"", "puts"}, id_call{"", "call"},
            id_isDefined{"", "is_def"}, id_typenameCheck{"", "tpchk"}, id_equal{"", "equal"};

    void init(Constants &constants) {
        constants.add(id_inputs, std::make_shared<Inputs>());
        constants.add(id_print, std::make_shared<Print>());
        constants.add(id_puts, std::make_shared<Puts>());
        constants.add(id_call, std::make_shared<Call>());
        constants.add(id_isDefined, std::make_shared<IsDefined>());
        constants.add(id_typenameCheck, std::make_shared<TypenameCheck>());
        constants.add(id_equal, std::make_shared<Equal>());
    }
}