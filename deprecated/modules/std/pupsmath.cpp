//
// Created by impodog on 7/1/2023.
//

#include "pupsmath.h"

namespace pups::modules::math {
    ObjectPtr Pi = std::make_shared<numbers::FloatType>(M_PI);
    ObjectPtr E = std::make_shared<numbers::FloatType>(M_E);

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

    using FloatFunc = std::function<library::pups_float(library::pups_float)>;

    ObjectPtr get_float_func(const FloatFunc &core) {
        return std::make_shared<Function>([core](FunctionArgs &args, Map *map) -> ObjectPtr {
            if (args.size() != 1) {
                map->throw_error(
                        std::make_shared<library::ArgumentError>(
                                "math floating functions require one only argument."));
            } else {
                auto ptr = cast<numbers::FloatType>(*args.front());
                if (ptr)
                    return std::make_shared<numbers::FloatType>(core(ptr->value));
                else
                    map->throw_error(
                            std::make_shared<library::TypeError>("math floating functions require one only argument."));
            }
            return pending;
        });
    }

    library::pups_float radians(library::pups_float x) {
        return x * M_PI / 180;
    }

    library::pups_float degrees(library::pups_float x) {
        return x * 180 / M_PI;
    }

    library::pups_float float_sin(library::pups_float x) {
        return sin(x);
    }

    library::pups_float float_cos(library::pups_float x) {
        return cos(x);
    }

    library::pups_float float_tan(library::pups_float x) {
        return tan(x);
    }


    Id id_math = pups_std::get_std_lib_name("math");
    Id id_pi{"", "pi"}, id_e{"", "e"},
            id_ftoi{"", "ftoi"}, id_itof{"", "itof"}, id_round{"", "round"},
            id_sin{"", "sin"}, id_cos{"", "cos"}, id_tan{"", "tan"},
            id_radians{"", "rad"}, id_degrees{"", "deg"};

    void init(Constants &constants) {
        auto &math = constants.new_sub_const(id_math);
        math.add(id_pi, Pi);
        math.add(id_e, E);
        math.add(id_ftoi, std::make_shared<Function>(convert_int));
        math.add(id_itof, std::make_shared<Function>(convert_float));
        math.add(id_round, std::make_shared<Function>(round_int));
        math.add(id_sin, get_float_func(float_sin));
        math.add(id_tan, get_float_func(float_tan));
        math.add(id_cos, get_float_func(float_cos));
        math.add(id_radians, get_float_func(radians));
        math.add(id_degrees, get_float_func(degrees));
    }
}