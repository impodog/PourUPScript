//
// Created by impodog on 6/21/2023.
//

#include "string_code.h"

namespace pups::modules::string_code {
    using namespace library::builtins::function;
    using namespace pups::library::builtins;

    ObjectPtr find(FunctionArgs &args, Map *map) {
        if (args.size() != 1)
            map->throw_error(std::make_shared<library::ArgumentError>("Find function requires one only argument."));
        else {
            auto ptr = std::dynamic_pointer_cast<String>(*args.front());
            if (ptr) {
                return map->find(Id{ptr->data()}, map);
            } else
                map->throw_error(
                        std::make_shared<library::TypeError>("Find function requires a string argument."));
        }
        return pending;
    }

    Id id_find{"", "find"};

    ObjectPtr string_code_load(FunctionArgs &args, Map *map) {
        map->add_object(id_find, std::make_shared<Function>(find));
        return pending;
    }


    void init(Constants &constants) {
        auto string_code_func = std::make_shared<Function>(string_code_load);
        constants.add(pups_std::get_std_func_name("scode"), string_code_func);
        constants.add(module_link_name("scode"), string_code_func);
    }
}
