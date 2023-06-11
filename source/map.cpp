//
// Created by Dogs-Cute on 5/27/2023.
//

#include "map.h"

#include <utility>
#include "object.h"


namespace pups::library {
    size_t err_count = 0;

    Map *Map::deepest_sub_map() {
        auto map = m_sub_map;
        if (map) {
            while (map->m_sub_map) {
                map = map->m_sub_map;
            }
            return map;
        } else
            return this;
    }

    ObjectPtr &Map::bare_find(const Id &name, Map *deepest) {
        Map *map = deepest;
        while (map) {
            try {
                return map->m_map.at(name);
            } catch (const std::out_of_range &) {
                map = map->m_parent_map;
            }
        }
        deepest->add_object(name);
        return deepest->m_map.at(name);
    }

    ObjectPtr &Map::staged_find(std::queue<std::string> &parts, Map *deepest) {
        ObjectPtr *object = &bare_find(Id{parts.front()}, deepest);
        MapPtr tmp;
        parts.pop(); // since the first is used, pop it
        while (!parts.empty()) {
            tmp = std::dynamic_pointer_cast<Map>(*object);
            if (tmp) {
                deepest = tmp.get();
                object = &tmp->bare_find(Id{parts.front()}, deepest);
            } else {
                bool reput_this = false;
                auto next = &object->get()->find(Id{parts.front()}, this, &reput_this);
                if (reput_this)
                    m_pending_put.emplace(*next, *object);
                object = next;
            }
            parts.pop();
        }
        return *object;
    }

    void Map::copy_signs_from(Map *map) {
        signs.break_sign = map->signs.break_sign;
    }

    Map::~Map() {
        if (m_parent_map) {
            m_parent_map->m_sub_map = nullptr;
            while (!m_errors.empty()) {
                m_parent_map->m_errors.push(m_errors.front());
                m_errors.pop();
            }
        } else
            report_errs();
    }

    Map::Map(Map *parent_map) : m_parent_map(parent_map) {
        m_parent_map->m_sub_map = this;
    }

    ObjectPtr Map::put(ObjectPtr &object, Map *map) {
        if (m_sub_map)
            return m_sub_map->put(object, this);
        else {
            if (!m_return &&
                !signs.break_sign) { // when map is returned or broken, the map skips all the statements below
                while (!m_pending_put.empty()) {
                    m_pending_put.front().first->put(m_pending_put.front().second, this);
                    m_pending_put.pop();
                }
                if (m_base) {
                    auto ptr = m_base->put(object, this);
                    if (ptr) // when returning nullptr, m_base stays the same
                        m_base = ptr;
                } else {
                    m_base = object;
                }
            }
            return nullptr;
        }
    }

    ObjectPtr &Map::find(const Id &name, Map *map, bool *reput_this) {
        //std::cout << name.str() << std::endl;
        Map *deepest = deepest_sub_map();
        auto parts = name.split_by('.');
        if (parts.size() == 1)
            return bare_find(name, deepest);
        else
            return staged_find(parts, deepest);
    }

    void Map::throw_error(const ErrorPtr &error) {
        m_errors.push(error);
    }

    void Map::add_object(const Id &name, const ObjectPtr &object) {
        try {
            m_map.at(name) = object;
        } catch (const std::out_of_range &) {
            m_map.insert({name, object});
        }
    }

    void Map::set_object(const Id &name, const ObjectPtr &object) {
        find(name, this, nullptr) = object;
    }

    ObjectPtr Map::end_of_line(Map *map) {
        auto deepest = deepest_sub_map();
        if (deepest && deepest->m_base) {
            deepest->m_temp = deepest->m_base->end_of_line(map);
            deepest->m_base = nullptr;
        }
        while (!m_memory_stack.empty())
            m_memory_stack.pop();
        while (!m_pending_put.empty()) {
            m_pending_put.pop();
        }
        return pending;
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

    void Map::set_child(Map *sub_map) noexcept {
        if (m_sub_map && sub_map == nullptr)
            copy_signs_from(m_sub_map);
        m_sub_map = sub_map;
    }

    ObjectPtr &Map::add_to_memory_stack(const ObjectPtr &object) {
        m_memory_stack.push(object);
        return m_memory_stack.top();
    }

    void Map::report_errs() {
        while (!m_errors.empty()) {
            std::cout << m_errors.front()->get() << std::endl;
            err_count += 1;
            m_errors.pop();
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

    ObjectPtr &Object::find(const Id &name, Map *map, bool *reput_this) {
        auto true_name = template_name(name.str(), {type_name()});
        if (reput_this)
            *reput_this = true;
        return map->bare_find(true_name, map);
    }

    void Map::Signs::set_break_sign(ObjectPtr object) {
        if (!break_sign || !object)
            break_sign = std::move(object);
    }

    ObjectPtr Error::put(ObjectPtr &object, Map *map) {
        map->throw_error(std::make_shared<TypeError>("Putting into Error is not allowed."));
        return pending;
    }

    ObjectPtr Pending::put(ObjectPtr &object, Map *map) {
        map->throw_error(std::make_shared<TypeError>("Putting into Pending is not allowed."));
        return pending;
    }

    ObjectPtr LongStr::put(ObjectPtr &object, Map *map) {
        map->throw_error(std::make_shared<TypeError>("Putting into LongStr is not allowed."));
        return pending;
    }

    ObjectPtr Symbol::put(ObjectPtr &object, Map *map) {
        map->throw_error(std::make_shared<TypeError>("Putting into Symbol is not allowed."));
        return pending;
    }
}
