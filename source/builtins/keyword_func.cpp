//
// Created by Dogs-Cute on 5/28/2023.
//

#include "keyword_func.h"
#include "numbers.h"

namespace pups::library::builtins::keyword_func {

    Print::Print() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        while (!args.empty()) {
            std::cout << args.front()->get()->str() << " ";
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
        return map->get_temp();
    }) {}

    If::If(bool require_false) : Function([require_false](FunctionArgs &args, Map *map) -> ObjectPtr {
        bool result = false;
        if (args.size() != 1 || !args.front()->get()->is_long_str())
            map->throw_error(
                    std::make_shared<ArgumentError>("Incorrect argument for keyword if. One long string is required."));
        else {
            if (map->get_temp()->condition() ^ require_false) {
                MapPtr sub_map = std::make_shared<Map>(map);
                Control control(*std::static_pointer_cast<LongStr>(*args.front())->ids(), sub_map);
                control.run();
                result = true;
            }
        }
        return std::make_shared<numbers::NumType<bool>>(result);
    }) {}

    While::While(bool require_false) : Function([require_false](FunctionArgs &args, Map *map) -> ObjectPtr {
        if (args.size() != 1 || !args.front()->get()->is_long_str())
            map->throw_error(
                    std::make_shared<ArgumentError>(
                            "Incorrect argument for keyword while. One long string is required."));
        else {
            ObjectPtr return_v = pending;
            MapPtr sub_map = std::make_shared<Map>(map);
            Control control(*std::static_pointer_cast<LongStr>(*args.front())->ids(), sub_map);
            while (return_v == pending || (return_v->condition() ^ require_false)) {
                control.run();
                return_v = sub_map->get_return();
                if (return_v == nullptr) return_v = pending;

                sub_map = std::make_shared<Map>(sub_map->get_parent());
                control.restart(sub_map);
            }
        }
        return map->get_temp();
    }) {}

    Return::Return() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        if (args.empty())
            map->get_return() = map->get_temp();
        else
            map->throw_error(std::make_shared<ArgumentError>("No arguments should be given to keyword return."));
        return pending;
    }) {}

    Targeting::Targeting() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        if (args.size() == 1 && *args.front() != pending)
            return *args.front();
        else
            map->throw_error(std::make_shared<ArgumentError>("Explicit targeting requires one only non-pending argument."));
        return pending;
    }) {}

    Id id_print{"", "print"}, id_moveTo{"", "mov"}, id_ifTrue{"", "if"}, id_ifFalse{"", "else"},
            id_whileTrue{"", "while"}, id_whileFalse{"", "while_not"}, id_return{"", "return"},
            id_targeting{"", "targeting"};

    void init(Constants &constants) {
        constants.add(id_print, std::make_shared<Print>());
        constants.add(id_moveTo, std::make_shared<MoveTo>());
        constants.add(id_ifTrue, std::make_shared<If>(false));
        constants.add(id_ifFalse, std::make_shared<If>(true));
        constants.add(id_whileTrue, std::make_shared<While>(false));
        constants.add(id_whileFalse, std::make_shared<While>(true));
        constants.add(id_return, std::make_shared<Return>());
        constants.add(id_targeting, std::make_shared<Targeting>());
    }
}