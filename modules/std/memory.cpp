//
// Created by impodog on 6/22/2023.
//

#include "memory.h"

namespace pups::modules::memory {
    ObjectPtr memory_swap(FunctionArgs &args, Map *map) {
        if (args.size() != 2)
            map->throw_error(std::make_shared<library::ArgumentError>("memory.swap requires two only arguments."));
        else {
            auto first = args.front();
            args.pop_front();
            auto second = args.front();
            args.pop_front();
            first->swap(*second);
        }
        return pending;
    }

    ObjectPtr memory_same(FunctionArgs &args, Map *map) {
        if (args.size() != 2)
            map->throw_error(std::make_shared<library::ArgumentError>("memory.same requires two only arguments."));
        else {
            auto first = args.front();
            args.pop_front();
            auto second = args.front();
            args.pop_front();
            return first->get() == second->get() ? numbers::True : numbers::False;
        }
        return pending;
    }

    Id id_memory{"", "memory"};
    Id id_memory_swap{"", "swap"}, id_memory_same{"", "same"};

    void init(pups::Constants &constants) {
        auto &memory = constants.new_sub_const(id_memory);
        memory.add(id_memory_swap, std::make_shared<Function>(memory_swap));
        memory.add(id_memory_same, std::make_shared<Function>(memory_same));
    }
}