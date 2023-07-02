//
// Created by impodog on 7/1/2023.
//

#include "typing.h"

#include <utility>

namespace pups::library::builtins::typing {

    Type::Type(Map *parent) : Map(parent, false) {}

    ObjectPtr &Type::find(const Id &name, Map *map) {
        try {
            return m_methods.at(name);
        } catch (const std::out_of_range &) {
            return Map::find(name, map);
        }
    }

    std::string Type::type_name() const noexcept {
        return "type";
    }

    std::string &Type::get_name() noexcept {
        return m_name;
    }

    IdSet &Type::get_attr() noexcept {
        return m_attr;
    }

    MethodMap &Type::get_methods() noexcept {
        return m_methods;
    }

    Id id_call{"", "call"};

    Instance::Instance(TypePtr type) : m_type(std::move(type)) {
        for (auto &name: m_type->get_attr())
            m_attr.insert({name, pending});
    }

    void Instance::add_methods(const ObjectPtr &self) {
        m_self_ptr = self;
        for (auto &method: m_type->get_methods()) {
            auto func = cast<Function>(method.second);
            if (func)
                m_attr.insert({method.first, std::make_shared<Function>(
                        [this, func](FunctionArgs &args, Map *map) -> ObjectPtr {
                            args.push_front(&this->m_self_ptr);
                            return func->get_core()(args, map);
                        }
                )});
        }
    }

    ObjectPtr Instance::put(ObjectPtr &object, Map *map) {
        return find(id_call, map)->put(object, map);
    }

    ObjectPtr Instance::end_of_line(Map *map) {
        return find(id_call, map)->end_of_line(map);
    }

    ObjectPtr &Instance::find(const Id &name, Map *map) {
        try {
            return m_attr.at(name);
        } catch (const std::out_of_range &) {}
        auto &result = m_type->map_find(name, map);
        if (!is_pending(result))
            return result;
        return Object::find(name, map);
    }

    std::string Instance::type_name() const noexcept {
        return m_type->get_name();
    }

    void init(Constants &constants) {

    }
}