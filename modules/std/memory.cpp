//
// Created by impodog on 6/22/2023.
//

#include "memory.h"

namespace pups::modules::memory {
    using namespace library::builtins::function;
    using namespace pups::library::builtins;

    ObjectPtr memory_swap(FunctionArgs &args, Map *map) {
        if (args.size() != 2)
            map->throw_error(std::make_shared<library::ArgumentError>("memory.swap requires two only arguments."));
        else {
            auto first = args.front();
            args.pop();
            auto second = args.front();
            args.pop();
            first->swap(*second);
        }
        return pending;
    }
    ObjectPtr memory_same(FunctionArgs &args, Map *map) {
        if (args.size() != 2)
            map->throw_error(std::make_shared<library::ArgumentError>("memory.same requires two only arguments."));
        else {
            auto first = args.front();
            args.pop();
            auto second = args.front();
            args.pop();
            return first->get() == second->get() ? numbers::True : numbers::False;
        }
        return pending;
    }
    Id id_memory_swap{"", "swap"}, id_memory_same{"", "same"};

    ObjectPtr memory_load(FunctionArgs &args, Map *map) {
        map->add_object(id_memory_swap, std::make_shared<Function>(memory_swap));
        map->add_object(id_memory_same, std::make_shared<Function>(memory_same));
        return pending;
    }

    void init(pups::Constants &constants) {
        auto memory_func = std::make_shared<Function>(memory_load);
        constants.add(pups_std::get_std_func_name("memory"), memory_func);
        constants.add(module_link_name("memory"), memory_func);
    }
}