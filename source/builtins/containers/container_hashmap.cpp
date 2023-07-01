//
// Created by impodog on 6/17/2023.
//

#include "container_hashmap.h"

namespace pups::library::builtins::containers {

    HashMap::HashMap(FunctionArgs &args) {
        push_args(args);
    }

    std::string HashMap::type_name() const noexcept {
        return name_hashmap;
    }

    std::string HashMap::str() const noexcept {
        std::string result = type_name() + "{";
        for (const auto &pair: data)
            result.append(pair.first->repr()).append(": ").append(pair.second->repr()).append(", ");
        if (!data.empty())
            result = result.substr(0, result.size() - 2);
        return result + "}";
    }

    void HashMap::push_args(FunctionArgs &args) {
        while (!args.empty()) {
            auto ptr = cast<Pair>(*args.front());
            if (ptr)
                data.insert({ptr->left, ptr->right});
            args.pop_front();
        }
    }

    bool HashMap::empty() const noexcept {
        return data.empty();
    }

    ObjectPtr HashMap::pop(const ObjectPtr &key) {
        ObjectPtr result = pending;
        try {
            result = data.at(key);
            data.erase(key);
        } catch (const std::out_of_range &) {}
        return result;
    }

    void HashMap::clear() noexcept {
        data.clear();
    }

    size_t HashMap::size() const noexcept {
        return data.size();
    }

    FunctionCore HashMap::get_method(const Id &name) {
        const auto &func = hashmap_cores.at(name);
        return [this, &func](FunctionArgs &args, Map *map) -> ObjectPtr {
            return func(*this, args, map);
        };
    }

    HashMapInit::HashMapInit() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        return std::make_shared<HashMap>(args);
    }) {}
}