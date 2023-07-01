//
// Created by impodog on 6/18/2023.
//

#include "random.h"
#include <random>
#include <numbers>

namespace pups::modules::random {
    std::random_device device;

    template<typename Type>
    void swap(Type &x, Type &y) {
        if (&x == &y) return;
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
                first = cast<numbers::IntType>(*args.front());
                if (first)
                    return std::make_shared<numbers::IntType>(device() % first->value);
                break;
            case 2:
                first = cast<numbers::IntType>(*args.front());
                args.pop_front();
                second = cast<numbers::IntType>(*args.front());
                args.pop_front();
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
                first = cast<numbers::FloatType>(*args.front());
                if (first)
                    engine = EngineType{0, first->value};
                else {
                    map->throw_error(
                            std::make_shared<library::TypeError>("Randomizing float should receive float arguments."));
                    return pending;
                }
                break;
            case 2:
                first = cast<numbers::FloatType>(*args.front());
                args.pop_front();
                second = cast<numbers::FloatType>(*args.front());
                args.pop_front();
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

    ObjectPtr random_choose_from(FunctionArgs &args, Map *map) {
        size_t pos = device() % args.size(), i = 0;
        while (!args.empty()) {
            if (i++ == pos)
                return *args.front();
            args.pop_front();
        }
        throw std::runtime_error("random.choose_from UNEXPECTED CODE PATH");;
    }

    ObjectPtr random_choice(FunctionArgs &args, Map *map) {
        if (args.size() != 1)
            map->throw_error(std::make_shared<library::ArgumentError>("random.choice requires one only argument."));
        else {
            auto ptr = cast<containers::Array>(*args.front());
            if (ptr)
                return ptr->data.at(device() % ptr->data.size());
            else
                map->throw_error(std::make_shared<library::TypeError>("random.choice requires an array argument."));
        }
        return pending;
    }

    ObjectPtr random_shuffle(FunctionArgs &args, Map *map) {
        if (args.size() != 1)
            map->throw_error(std::make_shared<library::ArgumentError>("random.shuffle requires one only argument."));
        else {
            auto ptr = cast<containers::Array>(*args.front());
            if (ptr) {
                size_t size = ptr->size();
                for (size_t i = 0; i < size; i++)
                    swap(ptr->data.at(i), ptr->data.at(device() % size));
            } else
                map->throw_error(std::make_shared<library::TypeError>("random.shuffle requires an array argument."));
        }
        return pending;
    }

    Id id_random{"", "random"};
    Id id_random_int{"", "rand_int"}, id_random_float{"", "rand_float"},
            id_random_choose_from{"", "choose_from"}, id_random_choice{"", "choice"},
            id_random_shuffle{"", "shuffle"};

    void init(pups::Constants &constants) {
        auto &random = constants.new_sub_const(id_random);
        random.add(id_random_int, std::make_shared<Function>(random_int));
        random.add(id_random_float, std::make_shared<Function>(random_float));
        random.add(id_random_choose_from, std::make_shared<Function>(random_choose_from));
        random.add(id_random_choice, std::make_shared<Function>(random_choice));
        random.add(id_random_shuffle, std::make_shared<Function>(random_shuffle));
    }
}