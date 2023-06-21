//
// Created by impodog on 6/21/2023.
//

#include "convert.h"

namespace pups::modules::convert {
    using namespace library::builtins::function;
    using namespace pups::library::builtins;

    ObjectPtr convert_int(FunctionArgs &args, Map *map) {
        if (args.size() != 1)
            map->throw_error(
                    std::make_shared<pups::library::ArgumentError>("Converting to int requires one only argument."));
        else {
            auto ptr = std::dynamic_pointer_cast<numbers::FloatType>(*args.front());
            if (ptr) {
                return std::make_shared<numbers::IntType>(
                        static_cast<library::pups_int>(ptr->value));
            } else
                map->throw_error(
                        std::make_shared<library::TypeError>("Converting to int requires one float argument."));
        }
        return pending;
    }

    ObjectPtr convert_float(FunctionArgs &args, Map *map) {
        if (args.size() != 1)
            map->throw_error(
                    std::make_shared<pups::library::ArgumentError>("Converting to float requires one only argument."));
        else {
            auto ptr = std::dynamic_pointer_cast<numbers::IntType>(*args.front());
            if (ptr) {
                return std::make_shared<numbers::FloatType>(
                        static_cast<library::pups_float>(ptr->value));
            } else
                map->throw_error(
                        std::make_shared<library::TypeError>("Converting to float requires one int argument."));
        }
        return pending;
    }

    ObjectPtr round_int(FunctionArgs &args, Map *map) {
        if (args.size() != 1)
            map->throw_error(
                    std::make_shared<pups::library::ArgumentError>("Rounding to int requires one only argument."));
        else {
            auto ptr = std::dynamic_pointer_cast<numbers::FloatType>(*args.front());
            if (ptr) {
                return std::make_shared<numbers::IntType>(
                        std::llround(ptr->value));
            } else
                map->throw_error(
                        std::make_shared<library::TypeError>("Rounding to int requires one float argument."));
        }
        return pending;
    }

    Id id_convert_int{"", "toi"}, id_convert_float{"", "tof"}, id_round_int{"", "round"};

    ObjectPtr convert_load(FunctionArgs &args, Map *map) {
        map->add_object(id_convert_int, std::make_shared<Function>(convert_int));
        map->add_object(id_convert_float, std::make_shared<Function>(convert_float));
        map->add_object(id_round_int, std::make_shared<Function>(round_int));
        return pending;
    }

    void init(pups::Constants &constants) {
        auto convert_func = std::make_shared<Function>(convert_load);
        constants.add(pups_std::get_std_func_name("convert"), convert_func);
        constants.add(module_link_name("convert"), convert_func);
    }
}