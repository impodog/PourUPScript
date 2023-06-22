//
// Created by impodog on 6/21/2023.
//

#include "string_code.h"

namespace pups::modules::string_code {
    using namespace library::builtins::function;
    using namespace pups::library::builtins;

    template<bool is_ref>
    ObjectPtr find(FunctionArgs &args, Map *map) {
        if (args.size() != 1)
            map->throw_error(std::make_shared<library::ArgumentError>("Find function requires one only argument."));
        else {
            auto ptr = std::dynamic_pointer_cast<String>(*args.front());
            if (ptr) {
                if constexpr (is_ref)
                    return std::make_shared<reference::Reference>(&map->find(Id{ptr->data()}, map)); 
                else
                    return map->find(Id{ptr->data()}, map);
            } else
                map->throw_error(
                        std::make_shared<library::TypeError>("Find function requires a string argument."));
        }
        return pending;
    }

    template<bool allow_underlined>
    ObjectPtr format_objects(FunctionArgs &args, Map *map) {
        if (!args.empty())
            map->throw_error(std::make_shared<library::ArgumentError>("Formatting objects does not require any arguments."));
        std::string result = "Format " + map->repr() + ":\n";
        for (auto &pair: map->get_all_objects()) {
            if (allow_underlined || pair.first.id().front() != '_') {
                    result.push_back('\t');
                    result.append(pair.first.str() + ": " + pair.second->repr()).push_back('\n');
                }
        }
        return std::make_shared<String>(result);
    }

    Id id_find{"", "find"}, id_find_ref{"", "find_ref"}
        , id_format{"", "format"}, id_format_all{"", "format_all"};

    ObjectPtr string_code_load(FunctionArgs &args, Map *map) {
        map->add_object(id_find, std::make_shared<Function>(find<false>));
        map->add_object(id_find_ref, std::make_shared<Function>(find<true>));
        map->add_object(id_format, std::make_shared<Function>(format_objects<false>));
        map->add_object(id_format_all, std::make_shared<Function>(format_objects<true>));
        return pending;
    }


    void init(Constants &constants) {
        auto string_code_func = std::make_shared<Function>(string_code_load);
        constants.add(pups_std::get_std_func_name("scode"), string_code_func);
        constants.add(module_link_name("scode"), string_code_func);
    }
}
