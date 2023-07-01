//
// Created by impodog on 6/21/2023.
//

#include "string_code.h"

namespace pups::modules::string_code {
    template<bool is_ref>
    ObjectPtr find(FunctionArgs &args, Map *map) {
        if (args.size() != 1)
            map->throw_error(std::make_shared<library::ArgumentError>("Find function requires one only argument."));
        else {
            auto ptr = cast<String>(*args.front());
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

    Id id_scode{"", "scode"};
    Id id_find{"", "find"}, id_find_ref{"", "find_ref"}
        , id_format{"", "format"}, id_format_all{"", "format_all"};


    void init(Constants &constants) {
        auto &scode = constants.new_sub_const(id_scode);
        scode.add(id_find, std::make_shared<Function>(find<false>));
        scode.add(id_find_ref, std::make_shared<Function>(find<true>));
        scode.add(id_format, std::make_shared<Function>(format_objects<false>));
        scode.add(id_format_all, std::make_shared<Function>(format_objects<true>));
    }
}
