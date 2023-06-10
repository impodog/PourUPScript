//
// Created by Dogs-Cute on 5/28/2023.
//

#include "keyword_func.h"
#include "numbers.h"

namespace pups::library::builtins::keyword_func {

    MoveTo::MoveTo() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        while (!args.empty()) {
            *args.front() = map->get_temp();
            args.pop();
        }
        return map->get_temp();
    }) {}

    If::If(bool require_false) : Function([require_false](FunctionArgs &args, Map *map) -> ObjectPtr {
        MapPtr sub_map;
        if (args.size() != 1 || !args.front()->get()->is_long_str())
            map->throw_error(
                    std::make_shared<ArgumentError>("Incorrect argument for keyword if. One long string is required."));
        else {
            if (map->get_temp()->condition() ^ require_false) {
                sub_map = std::make_shared<Map>(map);
                Control control(*std::static_pointer_cast<LongStr>(*args.front())->ids(), sub_map);
                control.run();
            }
        }
        return sub_map;
    }) {}

    While::While(bool require_false) : Function([require_false](FunctionArgs &args, Map *map) -> ObjectPtr {
        MapPtr sub_map;
        if (args.size() != 1 || !args.front()->get()->is_long_str())
            map->throw_error(
                    std::make_shared<ArgumentError>(
                            "Incorrect argument for keyword while. One long string is required."));
        else {
            ObjectPtr break_v = pending;
            sub_map = std::make_shared<Map>(map);
            Control control(*std::static_pointer_cast<LongStr>(*args.front())->ids(), sub_map);
            while (break_v == pending || (break_v->condition() ^ require_false)) {
                control.run();
                break_v = sub_map->signs.break_sign;
                if (break_v == nullptr) break_v = pending;

                sub_map = std::make_shared<Map>(sub_map->get_parent());
                control.restart(sub_map);
            }
            sub_map->signs.set_break_sign(nullptr);
        }
        return sub_map;
    }) {}

    Return::Return() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        if (args.empty())
            map->get_return() = map->get_temp();
        else
            map->throw_error(std::make_shared<ArgumentError>("No arguments should be given to keyword return."));
        return pending;
    }) {}

    With::With() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        if (args.size() == 1 && *args.front() != pending)
            return *args.front();
        else
            map->throw_error(
                    std::make_shared<ArgumentError>("Explicit with requires one only non-pending argument."));
        return pending;
    }) {}

    Clear::Clear() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        while (!args.empty()) {
            *args.front() = pending;
            args.pop();
        }
        return pending;
    }) {}

    Delete::Delete() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        while (!args.empty()) {
            args.front()->reset();
            args.pop();
        }
        return pending;
    }) {}

    Break::Break() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        if (args.size() != 1)
            map->throw_error(std::make_shared<ArgumentError>("Break function requires one only argument."));
        else {
            map->signs.set_break_sign(*args.front());
            return *args.front();
        }
        return pending;
    }) {}

    Unmap::Unmap() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        if (args.empty())
            args.push(&map->get_temp());
        while (!args.empty()) {
            auto ptr = std::dynamic_pointer_cast<Map>(*args.front());
            if (ptr) {
                map->copy_objects_from(ptr.get());
            } else {
                map->throw_error(std::make_shared<TypeError>(
                        "Unmap function requires map arguments(if no are given, temp is used instead)."));
                return pending;
            }
            args.pop();
        }
        return pending;
    }) {}

    Id id_moveTo{"", "mov"}, id_ifTrue{"", "if"}, id_ifFalse{"", "else"},
            id_whileTrue{"", "while"}, id_whileFalse{"", "while_not"}, id_return{"", "ret"},
            id_with{"", "with"}, id_clear{"", "clr"}, id_delete{"", "del"}, id_break{"", "break"},
            id_unmap{"", "unmap"};

    void init(Constants &constants) {
        constants.add(id_moveTo, std::make_shared<MoveTo>());
        constants.add(id_ifTrue, std::make_shared<If>(false));
        constants.add(id_ifFalse, std::make_shared<If>(true));
        constants.add(id_whileTrue, std::make_shared<While>(false));
        constants.add(id_whileFalse, std::make_shared<While>(true));
        constants.add(id_return, std::make_shared<Return>());
        constants.add(id_with, std::make_shared<With>());
        constants.add(id_clear, std::make_shared<Clear>());
        constants.add(id_delete, std::make_shared<Delete>());
        constants.add(id_break, std::make_shared<Break>());
        constants.add(id_unmap, std::make_shared<Unmap>());
    }
}