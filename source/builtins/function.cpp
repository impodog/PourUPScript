//
// Created by Dogs-Cute on 5/27/2023.
//

#include "function.h"

#include <utility>

namespace pups::library::builtins::function {
    Function::Function(FunctionCore core) : m_core(std::move(core)) {
        m_args.emplace();
    }

    ObjectPtr Function::put(ObjectPtr &object, Map *map) {
        try {
            m_args.at(map).push_back(&object);
        } catch (const std::out_of_range &) {
            m_args.insert({map, {}});
            m_args.at(map).push_back(&object);
        }
        return nullptr;
    }

    ObjectPtr Function::end_of_line(Map *map) {
        ObjectPtr result;
        try {
            result = m_core(m_args.at(map), map);
            m_args.erase(map);
        } catch (const std::out_of_range &) {
            FunctionArgs args;
            result = m_core(args, map);
        }
        return result;
    }

    std::string Function::type_name() const noexcept {
        return "func";
    }

    const FunctionCore &Function::get_core() const noexcept {
        return m_core;
    }

    ObjectPtr &HasMethods::find(const Id &name, Map *map) {
        try {
            return used_functions.at(name);
        } catch (const std::out_of_range &) {
            try {
                auto func = std::make_shared<Function>(get_method(name));
                used_functions.insert({name, func});
                return used_functions.at(name);
            } catch (const std::out_of_range &) {
                return Object::find(name, map);
            }
        }
    }

    void init(Constants &constants) {

    }

    ObjectPtr empty_func = std::make_shared<Function>([](FunctionArgs &args, Map *map) -> ObjectPtr {
        return pending;
    });
}