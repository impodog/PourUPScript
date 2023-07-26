//
// Created by Dogs-Cute on 5/27/2023.
//

#include "map.h"

#include <utility>
#include "object.h"


namespace pups::library {
    size_t err_count = 0;

    ObjectPtr &Map::local_find(const Id &name) {
        try {
            return m_map.at(name);
        } catch (const std::out_of_range &) {}
        try {
            return m_global->m_map.at(name);
        } catch (const std::out_of_range &) {}
        add_object(name);
        return m_map.at(name);
    }

    ObjectPtr &Map::bare_find(const Id &name) {
        Map *map = this;
        while (map) {
            try {
                return map->m_map.at(name);
            } catch (const std::out_of_range &) {
                map = map->m_parent_map;
            }
        }
        add_object(name);
        return m_map.at(name);
    }

    ObjectPtr &Map::single_find(const Id &name) {
        ObjectPtr *result;
        if (name.qual_has(remove_symbol)) {
            remove_object(name);
            return pending;
        } else if (name.qual_has(public_symbol))
            result = &bare_find(name);
        else
            result = &local_find(name);

        if (name.qual_has(unpack_symbol))
            unpack_array(*result);
        return *result;
    }

    ObjectPtr &Map::staged_find(std::queue<std::string> &parts, Map *map) {
        if (map == nullptr)
            map = this;
        ObjectPtr *object = &single_find(Id{parts.front()});
        MapPtr tmp;
        parts.pop(); // since the first is used, pop it
        while (!parts.empty()) {
            tmp = cast<Map>(*object);
            if (tmp) {
                object = &tmp->single_find(Id{parts.front()});
            } else {
                auto next = &object->get()->find(Id{parts.front()}, this);
                object = next;
            }
            parts.pop();
        }
        return *object;
    }

    void Map::copy_signs_from(Map *map) {
        signs.break_sign = map->signs.break_sign;
        if (map->m_upsearch_map != map)
            m_return = map->get_return();
    }

    void Map::map_put(ObjectPtr &object, Map *map) {
        if (m_base) {
            auto ptr = m_base->put(object, this);
            if (ptr) // when returning nullptr, m_base stays the same
                m_base = ptr;
        } else {
            m_base = object;
        }
    }

    void Map::map_end_of_line() {
        if (m_base) {
            m_temp = m_base->end_of_line(this);
            m_base = nullptr;
        }
        while (!m_memory_stack.empty())
            m_memory_stack.pop();
    }


    Map::~Map() {
        if (m_parent_map) {
            m_parent_map->m_sub_map = nullptr;
            push_up_errs();
        } else
            report_errs();
    }

    Map::Map(Map *parent_map, bool allow_upsearch) :
            m_parent_map(parent_map), m_global(parent_map->m_global) {
        if (allow_upsearch)
            // The default upsearch map is m_parent_map unless it's changed
            m_upsearch_map = m_parent_map->m_upsearch_map;
        m_parent_map->set_child(this);
    }

    Map::Map(Map *parent_map, Map *restore_map, bool allow_upsearch) :
            Map(parent_map, allow_upsearch) {
        m_restore_map = restore_map;
    }

    ObjectPtr Map::put(ObjectPtr &object, Map *map) {
        if (m_unpacked.empty())
            m_deepest->map_put(object, m_deepest);
        else {
            while (!m_unpacked.empty()) {
                m_deepest->map_put(*m_unpacked.front(), m_deepest);
                m_unpacked.pop();
            }
        }
        return nullptr;
    }

    ObjectPtr &Map::map_find(const Id &name, Map *map) {
        auto parts = name.split_by('.');
        if (parts.size() == 1)
            return m_upsearch_map->single_find(name);
        else
            return m_upsearch_map->staged_find(parts, this);
    }

    ObjectPtr &Map::source_find(const Id &name, Map *map) {
        //std::cout << "FINDING " << name.str() << std::endl;
        return m_deepest->map_find(name, map);
    }

    void Map::throw_error(const ErrorPtr &error) {
        if (!m_debug_info.empty()) {
            error->m_error = m_debug_info + ":\n\t" + error->m_error;
        }
        m_errors.push_back(error);
    }

    void Map::add_object(const Id &name, const ObjectPtr &object) {
        try {
            m_map.at(name) = object;
        } catch (const std::out_of_range &) {
            m_map.insert({name, object});
        }
    }

    void Map::set_object(const Id &name, const ObjectPtr &object) {
        find(name, this) = object;
    }

    void Map::remove_object(const Id &name) {
        try {
            m_map.erase(name);
        } catch (const std::out_of_range &) {
            throw_error(std::make_shared<IdError>("Cannot remove name \"" + name.str() + "\"."));
        }
    }

    ObjectPtr Map::end_of_line(Map *map) {
        m_deepest->map_end_of_line();
        return pending;
    }

    std::string Map::type_name() const noexcept {
        return "map";
    }

    // WARNING the result can be null
    ObjectPtr &Map::get_return() noexcept {
        return m_return;
    }

    ObjectPtr &Map::get_temp() noexcept {
        if (m_temp)
            return m_temp;
        else
            return pending;
    }

    Map *Map::get_parent() noexcept {
        return m_parent_map;
    }

    Map *Map::get_global() noexcept {
        return m_global;
    }


    void Map::set_child(Map *sub_map) noexcept {
        bool is_null = sub_map == nullptr;
        Map *map = is_null ? this : sub_map, *deepest;
        if (is_null) {
            if (m_sub_map)
                copy_signs_from(m_sub_map);
            deepest = this;
        } else {
            sub_map->m_parent_map = this;
            deepest = sub_map;
        }
        while (map) {
            map->m_deepest = deepest;
            map = map->m_parent_map;
        }
        m_sub_map = sub_map;
    }

    void Map::restore() noexcept {
        if (m_parent_map)
            m_parent_map->set_child(nullptr);
        if (m_restore_map)
            m_restore_map->set_child(nullptr);
    }

    ObjectPtr &Map::add_to_memory_stack(const ObjectPtr &object) {
        m_memory_stack.push(object);
        return m_memory_stack.top();
    }

    void Map::report_errs() {
        while (!m_errors.empty()) {
            std::cout << m_errors.front()->get() << std::endl;
            err_count += 1;
            m_errors.pop_front();
        }
    }

    void Map::inherit_errs(Map *map) {
        while (!map->m_errors.empty()) {
            m_errors.push_back(map->m_errors.front());
            map->m_errors.pop_front();
        }
        if (m_parent_map && m_parent_map->m_sub_map != this) {
            push_up_errs();
        }
    }

    void Map::push_up_errs() {
        if (m_parent_map) {
            m_parent_map->inherit_errs(this);
        }
    }

    void Map::copy_objects_from(Map *map) {
        if (map == this)
            return;
        for (auto &obj: map->m_map) {
            const auto &id = obj.first.id();
            if (id.empty() || id.front() != '_')
                add_object(obj.first, obj.second);
        }
    }

    void Map::change_debug_info(std::string info) {
        m_debug_info = std::move(info);
    }

    size_t Map::count_depth() const noexcept {
        const Map *map = this;
        size_t result = 1;
        while (map) {
            result++;
            map = map->m_parent_map;
        }
        return result;
    }

    bool Map::can_run() const noexcept {
        return !m_return && !signs.break_sign;
    }

    bool Map::can_run_while() const noexcept {
        return !m_return;
    }

    const ObjectMap &Map::get_all_objects() const noexcept {
        return m_map;
    }

    bool Map::catch_by(CatchRequirements &required) {
        while (!m_errors.empty()) {
            if (required.empty() || required.find(m_errors.back()->error_name()) != required.end()) {
                m_errors.clear();
                return true;
            }
            m_errors.pop_back();
        }
        return false;
    }

    ObjectPtr Object::end_of_line(Map *map) {
        return map->m_temp;
    }

    ObjectPtr &Object::source_find(const Id &name, Map *map) {
        throw FindError(this, name);
    }

    ObjectPtr &Object::find(const Id &name, Map *map) {
        try {
            return source_find(name, map);
        } catch (const FindError &err) {
            map->throw_error(std::make_shared<IdError>(
                    "Object " + err.sender->repr() + " cannot find name \"" + err.id.str() + "\"."));
            return pending;
        }
    }

    void Map::Signs::set_break_sign(ObjectPtr object) {
        if (!break_sign || !object)
            break_sign = std::move(object);
    }

    ObjectPtr Error::put(ObjectPtr &object, Map *map) {
        map->throw_error(std::make_shared<TypeError>("Putting " + object->repr() +
                                                     " into an Error is not allowed."));
        return pending;
    }

    ObjectPtr Pending::put(ObjectPtr &object, Map *map) {
        map->throw_error(std::make_shared<TypeError>("Putting " + object->repr() +
                                                     " into Pending is not allowed."));
        return nullptr;
    }

    ObjectPtr LongStr::put(ObjectPtr &object, Map *map) {
        map->throw_error(std::make_shared<TypeError>("Putting " + object->repr() +
                                                     " into LongStr is not allowed."));
        return nullptr;
    }

    ObjectPtr Symbol::put(ObjectPtr &object, Map *map) {
        map->throw_error(std::make_shared<TypeError>(
                "Putting " + object->repr() + " into Symbol \"" + m_name + "\" is not allowed."));
        return nullptr;
    }
}
