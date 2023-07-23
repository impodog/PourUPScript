//
// Created by impodog on 7/23/2023.
//

#include "container_deque.h"

namespace pups::library::builtins::containers {
    Deque::Deque(FunctionArgs &args) {
        push_back_args(args);
    }

    Deque::Deque(std::deque<ObjectPtr> args) : data(std::move(args)) {

    }

    std::string Deque::type_name() const noexcept {
        return "deque";
    }

    std::string Deque::str() const noexcept {
        std::string result = type_name() + "{";
        for (const auto &val: data)
            result.append(val->repr()).append(", ");
        if (!data.empty())
            result = result.substr(0, result.size() - 2);
        result.push_back('}');
        return result;
    }

    void Deque::push_back_args(FunctionArgs &args) {
        while (!args.empty()) {
            data.push_back(*args.front());
            args.pop_front();
        }
    }

    void Deque::push_front_args(FunctionArgs &args) {
        while (!args.empty()) {
            data.push_front(*args.front());
            args.pop_front();
        }
    }

    bool Deque::empty() const noexcept {
        return data.empty();
    }

    ObjectPtr &Deque::back() {
        return data.back();
    }

    ObjectPtr &Deque::front() {
        return data.front();
    }

    ObjectPtr Deque::pop_back() {
        ObjectPtr tmp = data.back();
        data.pop_back();
        return tmp;
    }

    ObjectPtr Deque::pop_front() {
        ObjectPtr tmp = data.front();
        data.pop_front();
        return tmp;
    }

    void Deque::clear() noexcept {
        data.clear();
    }

    size_t Deque::size() const noexcept {
        return data.size();
    }

    FunctionCore Deque::get_method(const Id &name) {
        const auto &func = deque_cores.at(name);
        return [this, &func](FunctionArgs &args, Map *map) -> ObjectPtr {
            return func(*this, args, map);
        };
    }

    DequeInit::DequeInit() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        return std::make_shared<Deque>(args);
    }) {

    }
}

