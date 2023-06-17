//
// Created by impodog on 6/17/2023.
//

#include "containers.h"

namespace pups::library::builtins::containers {
    template<typename Container>
    using ContainerOperatorCore = std::function<ObjectPtr(const std::shared_ptr<Container> &, FunctionArgs &, Map *)>;

    template<typename Container>
    void constant_add(Constants &constants, const Id &name, const ContainerOperatorCore<Container> &core) {
        constants.add(name, std::make_shared<Function>([core](FunctionArgs &args, Map *map) -> ObjectPtr {
            auto ptr = std::dynamic_pointer_cast<Container>(*args.front());
            if (ptr) {
                args.pop();
                return core(ptr, args, map);
            } else
                map->throw_error(std::make_shared<ArgumentError>("Container operator did not get the correct type."));
            return pending;
        }));
    }

    template<typename Container>
    ObjectPtr container_push(const std::shared_ptr<Container> &container, FunctionArgs &args, Map *map) {
        auto result = *args.back();
        container->push_args(args);
        return result;
    }

    template<typename Container>
    ObjectPtr container_pop_back(const std::shared_ptr<Container> &container, FunctionArgs &args, Map *map) {
        auto back = !container->empty() ? container->back() : pending;
        if (container->empty())
            map->throw_error(std::make_shared<ValueError>(
                    "Container.pop_back(or simply .pop) cannot perform without elements."));
        else
            container->pop_back();
        if (!args.empty())
            map->throw_error(std::make_shared<ArgumentError>(
                    "Container.pop_back(or simply .pop) does not require any arguments."));
        return back;
    }

    template<typename Container>
    ObjectPtr container_pop_at(const std::shared_ptr<Container> &container, FunctionArgs &args, Map *map) {
        ObjectPtr result;
        while (!args.empty()) {
            auto ptr = std::dynamic_pointer_cast<numbers::IntType>(*args.front());
            if (ptr)
                try {
                    result = container->pop_at(ptr->value);
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
    ObjectPtr container_pop_key(const std::shared_ptr<Container> &container, FunctionArgs &args, Map *map) {
        if (args.size() != 1)
            map->throw_error(
                    std::make_shared<ArgumentError>("Container.pop_key(or simply .pop) requires one only argument"));
        else {
            auto result = container->pop(*args.front());
            if (result.get() == pending.get())
                map->throw_error(std::make_shared<OutOfBoundError>(
                        "Container.pop_key(or simply .pop) cannot find the desired key."));
            return result;
        }
        return pending;
    }

    template<typename Container>
    ObjectPtr container_clear(const std::shared_ptr<Container> &container, FunctionArgs &args, Map *map) {
        container->data.clear();
        if (!args.empty())
            map->throw_error(std::make_shared<ArgumentError>(
                    "Container.clear does not require any arguments."));
        return pending;
    }

    template<typename Container, bool return_reference>
    ObjectPtr container_at_index(const std::shared_ptr<Container> &container, FunctionArgs &args, Map *map) {
        if (args.size() != 1) {
            map->throw_error(
                    std::make_shared<ArgumentError>("Container.at requires one only argument."));
        } else {
            auto ptr = std::dynamic_pointer_cast<numbers::IntType>(*args.front());
            if (ptr) {
                try {
                    if constexpr (return_reference)
                        return std::make_shared<reference::Reference>(&container->data.at(ptr->value));
                    else
                        return container->data.at(ptr->value);
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
    ObjectPtr container_at_obj(const std::shared_ptr<Container> &container, FunctionArgs &args, Map *map) {
        if (args.size() != 1) {
            map->throw_error(
                    std::make_shared<ArgumentError>("Container.at requires one only argument."));
        } else {
            auto ptr = *args.front();
            try {
                if constexpr (return_reference)
                    return std::make_shared<reference::Reference>(&container->data.at(ptr));
                else
                    return container->data.at(ptr);
            } catch (const std::out_of_range &exc) {
                map->throw_error(
                        std::make_shared<OutOfBoundError>("Container.at out of bounds with key " + ptr->str() + "."));
            }
        }
        return pending;
    }

    template<typename Container>
    ObjectPtr container_size(const std::shared_ptr<Container> &container, FunctionArgs &args, Map *map) {
        if (!args.empty())
            map->throw_error(std::make_shared<ArgumentError>(
                    "Container.size does not require any arguments."));
        return std::make_shared<numbers::IntType>(container->size());
    }

    template<typename Container>
    ObjectPtr container_insert(const std::shared_ptr<Container> &container, FunctionArgs &args, Map *map) {
        auto ptr = std::dynamic_pointer_cast<numbers::IntType>(*args.front());
        args.pop();
        if (ptr) {
            while (!args.empty()) {
                container->insert_at(*args.front(), ptr->value);
                args.pop();
            }
        } else
            map->throw_error(
                    std::make_shared<TypeError>("Container.insert requires an integer as the first argument."));
        return pending;
    }

    template<typename Container>
    ObjectPtr container_get_keys(const std::shared_ptr<Container> &container, FunctionArgs &args, Map *map) {
        if (!args.empty())
            map->throw_error(std::make_shared<ArgumentError>("Container.get_keys requires no arguments"));
        std::vector<ObjectPtr> keys;
        for (const auto &pair: container->data)
            keys.push_back(pair.first);
        return std::make_shared<Array>(keys);
    }

    Id id_arrayInit{"", "array"}, id_pairInit{"", "pair"}, id_hashmapInit{"", "hashmap"};

    void init(Constants &constants) {
        constants.add(id_arrayInit, std::make_shared<ArrayInit>());
        constants.add(id_pairInit, std::make_shared<PairInit>());
        constants.add(id_hashmapInit, std::make_shared<HashMapInit>());

        constant_add<Array>(constants, template_name("push", {name_array}), container_push<Array>);
        constant_add<Array>(constants, template_name("pop", {name_array}), container_pop_back<Array>);
        constant_add<Array>(constants, template_name("pop_at", {name_array}), container_pop_at<Array>);
        constant_add<Array>(constants, template_name("clear", {name_array}), container_clear<Array>);
        constant_add<Array>(constants, template_name("at", {name_array}), container_at_index<Array, false>);
        constant_add<Array>(constants, template_name("at_ref", {name_array}), container_at_index<Array, true>);
        constant_add<Array>(constants, template_name("get_size", {name_array}), container_size<Array>);
        constant_add<Array>(constants, template_name("insert", {name_array}), container_insert<Array>);

        constant_add<HashMap>(constants, template_name("push", {name_hashmap}), container_push<HashMap>);
        constant_add<HashMap>(constants, template_name("pop", {name_hashmap}), container_pop_key<HashMap>);
        constant_add<HashMap>(constants, template_name("clear", {name_hashmap}), container_clear<HashMap>);
        constant_add<HashMap>(constants, template_name("at", {name_hashmap}), container_at_obj<HashMap, false>);
        constant_add<HashMap>(constants, template_name("at_ref", {name_hashmap}), container_at_obj<HashMap, true>);
        constant_add<HashMap>(constants, template_name("get_size", {name_hashmap}), container_size<HashMap>);
        constant_add<HashMap>(constants, template_name("get_keys", {name_hashmap}), container_get_keys<HashMap>);
    }
}