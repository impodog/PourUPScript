//
// Created by Dogs-Cute on 5/27/2023.
//

#include "map.h"

#include <utility>
#include "object.h"


namespace pups::library {
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

    Map::~Map() {
        if (m_parent_map) {
            m_parent_map->m_sub_map = nullptr;
            while (!m_errors.empty()) {
                m_parent_map->m_errors.push(m_errors.front());
                m_errors.pop();
            }
        } else {
            while (!m_errors.empty()) {
                std::cerr << m_errors.front()->get() << std::endl;
                m_errors.pop();
            }
        }
    }

    Map::Map(Map *parent_map) : m_parent_map(parent_map) {
        m_parent_map->m_sub_map = this;
    }

    ObjectPtr Map::put(ObjectPtr &object, Map *map) {
        if (m_sub_map)
            return m_sub_map->put(object, this);
        else {
            if (!m_return) { // when returned, the map skips all the statements below
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

    ObjectPtr &Map::get(const Id &name) {
        Map *deepest = deepest_sub_map(), *map = deepest;
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
        get(name) = object;
    }

    ObjectPtr Map::end_of_line(Map *map) {
        auto deepest = deepest_sub_map();
        if (deepest && deepest->m_base) {
            deepest->m_temp = deepest->m_base->end_of_line(map);
            deepest->m_base = nullptr;
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
        m_sub_map = sub_map;
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
