//
// Created by impodog on 6/21/2023.
//

#include "convert.h"

namespace pups::modules::convert {
    ObjectPtr convert_int(FunctionArgs &args, Map *map) {
        if (args.size() != 1)
            map->throw_error(
                    std::make_shared<pups::library::ArgumentError>("Converting to int requires one only argument."));
        else {
            auto ptr = cast<numbers::FloatType>(*args.front());
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
            auto ptr = cast<numbers::IntType>(*args.front());
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
            auto ptr = cast<numbers::FloatType>(*args.front());
            if (ptr) {
                return std::make_shared<numbers::IntType>(
                        std::llround(ptr->value));
            } else
                map->throw_error(
                        std::make_shared<library::TypeError>("Rounding to int requires one float argument."));
        }
        return pending;
    }

    Id id_convert{"", "convert"};
    Id id_convert_int{"", "ftoi"}, id_convert_float{"", "itof"}, id_round_int{"", "round"};

    void init(pups::Constants &constants) {
        auto &convert = constants.new_sub_const(id_convert);
        convert.add(id_convert_int, std::make_shared<Function>(convert_int));
        convert.add(id_convert_float, std::make_shared<Function>(convert_float));
        convert.add(id_round_int, std::make_shared<Function>(round_int));
    }
}