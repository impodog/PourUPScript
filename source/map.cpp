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
        if (name.qual_has('&'))
            return bare_find(name);
        else
            return local_find(name);
    }

    ObjectPtr &Map::staged_find(std::queue<std::string> &parts, Map *map) {
        if (map == nullptr)
            map = this;
        ObjectPtr *object = &single_find(Id{parts.front()});
        MapPtr tmp;
        parts.pop(); // since the first is used, pop it
        while (!parts.empty()) {
            tmp = std::dynamic_pointer_cast<Map>(*object);
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

    Map::Map(Map *parent_map, bool allow_upsearch) :
            m_parent_map(parent_map), m_global(parent_map->m_global) {
        if (allow_upsearch)
            // The default upsearch map is m_parent_map unless it's changed
            m_upsearch_map = m_parent_map->m_upsearch_map;
        m_parent_map->set_child(this);
    }

    ObjectPtr Map::put(ObjectPtr &object, Map *map) {
        if (m_deepest != this)
            return m_deepest->put(object, m_deepest);
        else {
            if (!m_return && !signs.break_sign) {
                /* when map is returned or broken,
                * the map skips all the statements below*/
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

    ObjectPtr &Map::find(const Id &name, Map *map) {
        //std::cout << "FINDING " << name.str() << std::endl;
        auto parts = name.split_by('.');
        if (parts.size() == 1)
            return m_upsearch_map->single_find(name);
        else
            return m_upsearch_map->staged_find(parts, this);
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
        find(name, this) = object;
    }

    ObjectPtr Map::end_of_line(Map *map) {
        if (m_deepest && m_deepest->m_base) {
            m_deepest->m_temp = m_deepest->m_base->end_of_line(map);
            m_deepest->m_base = nullptr;
        }
        while (!m_memory_stack.empty())
            m_memory_stack.pop();
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

    size_t Map::count_depth() const noexcept {
        const Map *map = this;
        size_t result = 1;
        while (map) {
            result++;
            map = map->m_sub_map;
        }
        return result;
    }

    ObjectPtr &Object::find(const Id &name, Map *map) {
        map->throw_error(std::make_shared<IdError>("Object " + str() + " cannot find name \"" + name.str() + "\"."));
        return pending;
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
        map->throw_error(std::make_shared<TypeError>("Putting into Symbol \"" + m_name + "\" is not allowed."));
        return pending;
    }
}
