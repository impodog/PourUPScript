//
// Created by impodog on 6/17/2023.
//

#include "container_array.h"

namespace pups::library::builtins::containers {
    Array::Array(FunctionArgs &args) {
        push_args(args);
    }

    Array::Array(std::vector<ObjectPtr> args) : data(std::move(args)) {}

    std::string Array::type_name() const noexcept {
        return name_array;
    }

    std::string Array::str() const noexcept {
        std::string result = type_name() + "{";
        for (const auto &val: data)
            result.append(val->repr()).append(", ");
        if (!data.empty())
            result = result.substr(0, result.size() - 2);
        result.push_back('}');
        return result;
    }

    void Array::push_args(FunctionArgs &args) {
        while (!args.empty()) {
            data.push_back(*args.front());
            args.pop_front();
        }
    }

    bool Array::empty() const noexcept {
        return data.empty();
    }

    ObjectPtr &Array::back() {
        return data.back();
    }

    ObjectPtr Array::pop_back() {
        ObjectPtr tmp = data.back();
        data.pop_back();
        return tmp;
    }

    ObjectPtr Array::pop_at(size_t pos) {
        ObjectPtr tmp = data.at(pos);
        data.erase(data.begin() + static_cast<long long>(pos));
        return tmp;
    }

    void Array::clear() noexcept {
        data.clear();
    }

    size_t Array::size() const noexcept {
        return data.size();
    }

    void Array::insert_at(const ObjectPtr &obj, size_t pos) {
        data.insert(data.cbegin() + static_cast<long long>(pos), obj);
    }

    FunctionCore Array::get_method(const Id &name) {
        const auto &func = array_cores.at(name);
        return [this, &func](FunctionArgs &args, Map *map) -> ObjectPtr {
            return func(*this, args, map);
        };
    }

    ArrayInit::ArrayInit() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        return std::make_shared<Array>(args);
    }) {}
}