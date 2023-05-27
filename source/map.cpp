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
                m_parent_map->m_errors.push(m_errors.top());
                m_errors.pop();
            }
        } else {
            while (!m_errors.empty()) {
                std::cerr << m_errors.top()->get() << std::endl;
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
            if (m_base) {
                auto ptr = m_base->put(object, this);
                if (ptr)
                    m_base = ptr;
            } else {
                m_base = object;
            }
            return nullptr;
        }
    }

    ObjectPtr &Map::get(const Id &name) {
        Map *map = this;
        while (map) {
            try {
                return map->m_map.at(name);
            } catch (const std::out_of_range &) {
                map = map->m_parent_map;
            }
        }
        throw_error(std::make_shared<IdError>("Identifier " + name.str() + " not found."));
        return pending;
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

    ObjectPtr Map::end_of_line(Map *map) {
        deepest_sub_map()->m_base = nullptr;
        return pending;
    }

    ObjectPtr Error::put(ObjectPtr &object, Map *map) {
        map->throw_error(std::make_shared<TypeError>("Putting into Error is not allowed."));
        return pending;
    }

    ObjectPtr Pending::put(ObjectPtr &object, Map *map) {
        map->throw_error(std::make_shared<TypeError>("Putting into Pending is not allowed."));
        return pending;
    }

    LongStr::LongStr(IdFile idFile) : m_idFile(std::move(idFile)) {

    }

    ObjectPtr LongStr::put(ObjectPtr &object, Map *map) {
        map->throw_error(std::make_shared<TypeError>("Putting into LongStr is not allowed."));
        return pending;
    }

    const IdFile &LongStr::get() const noexcept {
        return m_idFile;
    }

    bool LongStr::is_long_str() const noexcept {
        return true;
    }
}
