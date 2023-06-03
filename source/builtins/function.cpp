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
            m_args.at(map).push(&object);
        } catch (const std::out_of_range &) {
            m_args.insert({map, {}});
            m_args.at(map).push(&object);
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

    void init_function() {

    }
}