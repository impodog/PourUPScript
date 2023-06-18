//
// Created by impodog on 6/17/2023.
//

#include "containers.h"

namespace pups::library::builtins::containers {

    template<typename Container>
    ObjectPtr container_push(Container &container, FunctionArgs &args, Map *map) {
        auto result = *args.back();
        container.push_args(args);
        return result;
    }

    template<typename Container>
    ObjectPtr container_pop_back(Container &container, FunctionArgs &args, Map *map) {
        auto back = !container.empty() ? container.back() : pending;
        if (container.empty())
            map->throw_error(std::make_shared<ValueError>(
                    "Container.pop_back(or simply .pop) cannot perform without elements."));
        else
            container.pop_back();
        if (!args.empty())
            map->throw_error(std::make_shared<ArgumentError>(
                    "Container.pop_back(or simply .pop) does not require any arguments."));
        return back;
    }

    template<typename Container>
    ObjectPtr container_pop_at(Container &container, FunctionArgs &args, Map *map) {
        ObjectPtr result;
        while (!args.empty()) {
            auto ptr = std::dynamic_pointer_cast<numbers::IntType>(*args.front());
            if (ptr)
                try {
                    result = container.pop_at(ptr->value);
                } catch (const std::out_of_range &) {
                    map->throw_error(std::make_shared<TypeError>(
                            "Container.pop_at cannot find value at " + ptr->str() + "."));
                }
            else
                map->throw_error(std::make_shared<TypeError>(
                        "Container.pop_at requires integers. Value " + args.front()->get()->str() + " skipped."));
            args.pop();
        }
        return result;
    }

    template<typename Container>
    ObjectPtr container_pop_key(Container &container, FunctionArgs &args, Map *map) {
        if (args.size() != 1)
            map->throw_error(
                    std::make_shared<ArgumentError>("Container.pop_key(or simply .pop) requires one only argument"));
        else {
            auto result = container.pop(*args.front());
            if (result.get() == pending.get())
                map->throw_error(std::make_shared<OutOfBoundError>(
                        "Container.pop_key(or simply .pop) cannot find the desired key."));
            return result;
        }
        return pending;
    }

    template<typename Container>
    ObjectPtr container_clear(Container &container, FunctionArgs &args, Map *map) {
        container.data.clear();
        if (!args.empty())
            map->throw_error(std::make_shared<ArgumentError>(
                    "Container.clear does not require any arguments."));
        return pending;
    }

    template<typename Container, bool return_reference>
    ObjectPtr container_at_index(Container &container, FunctionArgs &args, Map *map) {
        if (args.size() != 1) {
            map->throw_error(
                    std::make_shared<ArgumentError>("Container.at requires one only argument."));
        } else {
            auto ptr = std::dynamic_pointer_cast<numbers::IntType>(*args.front());
            if (ptr) {
                try {
                    if constexpr (return_reference)
                        return std::make_shared<reference::Reference>(&container.data.at(ptr->value));
                    else
                        return container.data.at(ptr->value);
                } catch (const std::out_of_range &exc) {
                    map->throw_error(
                            std::make_shared<OutOfBoundError>(
                                    "Container.at out of bounds with value " + ptr->str() + "."));
                }
            } else
                map->throw_error(
                        std::make_shared<TypeError>("Container.at requires an integer."));
        }
        return pending;
    }

    template<typename Container, bool return_reference>
    ObjectPtr container_at_obj(Container &container, FunctionArgs &args, Map *map) {
        if (args.size() != 1) {
            map->throw_error(
                    std::make_shared<ArgumentError>("Container.at requires one only argument."));
        } else {
            auto ptr = *args.front();
            try {
                if constexpr (return_reference)
                    return std::make_shared<reference::Reference>(&container.data.at(ptr));
                else
                    return container.data.at(ptr);
            } catch (const std::out_of_range &exc) {
                map->throw_error(
                        std::make_shared<OutOfBoundError>("Container.at out of bounds with key " + ptr->str() + "."));
            }
        }
        return pending;
    }

    template<typename Container>
    ObjectPtr container_size(Container &container, FunctionArgs &args, Map *map) {
        if (!args.empty())
            map->throw_error(std::make_shared<ArgumentError>(
                    "Container.size does not require any arguments."));
        return std::make_shared<numbers::IntType>(container.size());
    }

    template<typename Container>
    ObjectPtr container_insert(Container &container, FunctionArgs &args, Map *map) {
        auto ptr = std::dynamic_pointer_cast<numbers::IntType>(*args.front());
        args.pop();
        if (ptr) {
            while (!args.empty()) {
                container.insert_at(*args.front(), ptr->value);
                args.pop();
            }
        } else
            map->throw_error(
                    std::make_shared<TypeError>("Container.insert requires an integer as the first argument."));
        return pending;
    }

    template<typename Container>
    ObjectPtr container_get_keys(Container &container, FunctionArgs &args, Map *map) {
        if (!args.empty())
            map->throw_error(std::make_shared<ArgumentError>("Container.get_keys requires no arguments"));
        std::vector<ObjectPtr> keys(container.size());
        for (const auto &pair: container.data)
            keys.push_back(pair.first);
        return std::make_shared<Array>(keys);
    }

    Id id_arrayInit{"", "array"}, id_pairInit{"", "pair"}, id_hashmapInit{"", "hashmap"};

    const ContainerCoreMap<Array> array_cores = {
            {Id{"", "add"}, container_push<Array>},
            {Id{"", "pop"}, container_pop_back<Array>},
            {Id{"", "pop_at"}, container_pop_at<Array>},
            {Id{"", "clear"}, container_clear<Array>},
            {Id{"", "at"}, container_at_index<Array, false>},
            {Id{"", "at_ref"}, container_at_index<Array, true>},
            {Id{"", "get"}, container_at_index<Array, false>},
            {Id{"", "get_ref"}, container_at_index<Array, true>},
            {Id{"", "get_size"}, container_size<Array>},
            {Id{"", "insert"}, container_insert<Array>},
    };
    const ContainerCoreMap<HashMap> hashmap_cores = {
            {Id{"", "add"},      container_push<HashMap>},
            {Id{"", "pop"},      container_pop_key<HashMap>},
            {Id{"", "pop_at"},   container_pop_key<HashMap>},
            {Id{"", "clear"},    container_clear<HashMap>},
            {Id{"", "at"},       container_at_obj<HashMap, false>},
            {Id{"", "at_ref"},   container_at_obj<HashMap, true>},
            {Id{"", "get"},      container_at_obj<HashMap, false>},
            {Id{"", "get_ref"},  container_at_obj<HashMap, true>},
            {Id{"", "get_size"}, container_size<HashMap>},
            {Id{"", "get_keys"}, container_get_keys<HashMap>},
    };

    void init(Constants &constants) {
        constants.add(id_arrayInit, std::make_shared<ArrayInit>());
        constants.add(id_pairInit, std::make_shared<PairInit>());
        constants.add(id_hashmapInit, std::make_shared<HashMapInit>());
    }
}