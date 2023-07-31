//
// Created by impodog on 7/31/2023.
//

#include "enumerate.h"

namespace pups::library::builtins::enumerate {
    Enumerate::Enumerate(Map *parent_map) : Map(parent_map, false) {

    }

    ObjectPtr Enumerate::put(ObjectPtr &object, Map *map) {
        object = std::make_shared<numbers::IntType>(enumerate++);
        return nullptr;
    }

    EnumerateInit::EnumerateInit() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        if (args.empty()) {
            map->throw_error(
                    std::make_shared<ArgumentError>("enum type initialization requires at least one argument."));
        } else {
            auto ptr = cast<LongStr>(*args.back());
            if (ptr) {
                auto result = std::make_shared<Enumerate>(map);
                Control control(ptr->ids(), result);
                control.run();
                while (args.size() != 1) {
                    *args.front() = result;
                    args.pop_back();
                }
                return result;
            } else {
                map->throw_error(std::make_shared<TypeError>(
                        "enum type initialization requires a LongStr argument in the back."));
            }
        }
        return pending;
    }) {}

    Id id_enumerateInit{"", "enum"};

    void init(Constants &constants) {
        constants.add(id_enumerateInit, std::make_shared<EnumerateInit>());
    }
}