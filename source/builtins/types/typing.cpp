//
// Created by impodog on 7/1/2023.
//

#include "typing.h"

#include <utility>

namespace pups::library::builtins::typing {
    Id id_call = spec_name_of("called"), id_init_func = spec_name_of("init"), id_get_func = spec_name_of("get"),
            id_quit_func = spec_name_of("quit");

    size_t Type::hash::operator()(const TypePtr &type) const noexcept {
        return std::hash<std::string>()(type->m_name);
    }

    Type::Type(Map *parent) : Map(parent, false) {}

    ObjectPtr &Type::source_find(const Id &name, Map *map) {
        try {
            return m_methods.at(name);
        } catch (const std::out_of_range &) {
            try {
                auto &result = Map::source_find(name, map);
                if (!is_pending(result))
                    return result;
            } catch (const FindError &) {}

        }
        return Map::source_find(name, map);
    }

    std::string Type::str() const noexcept {
        return m_name;
    }

    std::string Type::repr() const noexcept {
        return "<" + type_name() + "[" + m_name + "]" + ":" + std::to_string(m_count) + " at " + to_string(this) + ">";
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

    void Type::inherit_from(const TypePtr &type) noexcept {
        m_parents.merge(type->m_parents);
        m_parents.insert(type);
        m_attr.merge(type->m_attr);
        m_methods.merge(type->m_methods);
    }

    bool Type::is_subtype(const TypePtr &type) const noexcept {
        return this == type.get() || m_parents.find(type) != m_parents.end();
    }

    Instance::Instance(TypePtr type) : m_type(std::move(type)) {
        for (auto &name: m_type->get_attr())
            m_attr.insert({name, pending});
    }

    void Instance::add_methods(const ObjectPtr &self, FunctionArgs &args, Map *map) {
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
        bool init_success = false;
        try {
            auto init_func = cast<Function>(source_find(id_init_func, map));
            if (init_func) {
                init_func->get_core()(args, map);
                init_success = true;
            }
        } catch (const FindError &) {}

        if (!init_success && !args.empty())
            map->throw_error(std::make_shared<ArgumentError>(
                    "No method \"" + id_init_func.str() + "\" found when initializing instance " + repr() +
                    ", but " + std::to_string(args.size()) + " arguments are given."));
    }

    ObjectPtr Instance::put(ObjectPtr &object, Map *map) {
        return find(id_call, map)->put(object, map);
    }

    ObjectPtr Instance::end_of_line(Map *map) {
        return find(id_call, map)->end_of_line(map);
    }

    ObjectPtr &Instance::source_find_non_get(const Id &name, Map *map) {
        try {
            return m_attr.at(name);
        } catch (const std::out_of_range &) {}
        auto &result = m_type->map_find(name, map);
        if (!is_pending(result))
            return result;
        return Object::source_find(name, map);
    }

    ObjectPtr &Instance::source_find(const Id &name, Map *map) {
        try {
            return source_find_non_get(name, map);
        } catch (const FindError &) {}
        try {
            auto ptr = cast<Function>(source_find_non_get(id_get_func, map));
            if (ptr) {
                return ptr->end_of_line(map)->source_find(name, map);
            }
        } catch (const FindError &) {}
        return Object::source_find(name, map);
    }

    std::string Instance::type_name() const noexcept {
        return m_type->get_name();
    }

    TypePtr Instance::get_type() const noexcept {
        return m_type;
    }

    void init(Constants &constants) {

    }
}