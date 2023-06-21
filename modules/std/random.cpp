//
// Created by impodog on 6/18/2023.
//

#include "random.h"
#include <random>
#include <numbers>

namespace pups::modules::random {
    using namespace pups::library::builtins::function;
    using namespace pups::library::builtins;
    std::random_device device;

    template<typename Type>
    void swap(Type &x, Type &y) {
        auto z = x;
        x = y;
        y = z;
    }

    ObjectPtr random_int(FunctionArgs &args, Map *map) {
        std::shared_ptr<numbers::IntType> first, second;
        switch (args.size()) {
            case 0:
                return std::make_shared<numbers::IntType>(device());
            case 1:
                first = std::dynamic_pointer_cast<numbers::IntType>(*args.front());
                if (first)
                    return std::make_shared<numbers::IntType>(device() % first->value);
                break;
            case 2:
                first = std::dynamic_pointer_cast<numbers::IntType>(*args.front());
                args.pop();
                second = std::dynamic_pointer_cast<numbers::IntType>(*args.front());
                args.pop();
                if (first->value > second->value)
                    swap(first, second);
                if (first && second)
                    return std::make_shared<numbers::IntType>(device() % (second->value - first->value) + first->value);
                break;
            default:
                map->throw_error(std::make_shared<library::ArgumentError>(
                        "Randomizing int should receive none, one or two arguments."));
        }
        map->throw_error(std::make_shared<library::TypeError>("Randomizing int should receive int arguments."));
        return pending;
    }

    ObjectPtr random_float(FunctionArgs &args, Map *map) {
        using EngineType = std::uniform_real_distribution<pups::library::pups_float>;
        std::shared_ptr<numbers::FloatType> first, second;
        EngineType engine;
        switch (args.size()) {
            case 0:
                engine = EngineType{0, 1};
                break;
            case 1:
                first = std::dynamic_pointer_cast<numbers::FloatType>(*args.front());
                if (first)
                    engine = EngineType{0, first->value};
                else {
                    map->throw_error(
                            std::make_shared<library::TypeError>("Randomizing float should receive float arguments."));
                    return pending;
                }
                break;
            case 2:
                first = std::dynamic_pointer_cast<numbers::FloatType>(*args.front());
                args.pop();
                second = std::dynamic_pointer_cast<numbers::FloatType>(*args.front());
                args.pop();
                if (first && second) {
                    if (first->value > second->value)
                        swap(first, second);
                    engine = EngineType{first->value, second->value};
                    break;
                } else {
                    map->throw_error(
                            std::make_shared<library::TypeError>("Randomizing float should receive float arguments."));
                    return pending;
                }
            default:
                map->throw_error(std::make_shared<library::ArgumentError>(
                        "Randomizing float should receive none, one or two arguments."));
                return pending;
        }
        return std::make_shared<numbers::FloatType>(engine(device));
    }

    Id id_random_int{"", "rand_int"}, id_random_float{"", "rand_float"};

    ObjectPtr random_load(FunctionArgs &args, Map *map) {
        map->add_object(id_random_int, std::make_shared<Function>(random_int));
        map->add_object(id_random_float, std::make_shared<Function>(random_float));
        return pending;
    }

    void init(pups::Constants &constants) {
        auto random_func = std::make_shared<Function>(random_load);
        constants.add(pups_std::get_std_func_name("random"), random_func);
        constants.add(module_link_name("random"), random_func);
    }
}