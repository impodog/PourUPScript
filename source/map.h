//
// Created by Dogs-Cute on 5/27/2023.
//

#ifndef PUPS_LIB_TESTS_MAP_H
#define PUPS_LIB_TESTS_MAP_H

#include "error.h"

namespace pups::library {
    using ObjectMap = std::unordered_map<Id, ObjectPtr, Id::hash>;

    class Map : public Object {
    protected:
        std::queue<ErrorPtr> m_errors;
        ObjectMap m_map;
        ObjectPtr m_base, m_return, m_temp;
        Map *m_sub_map = nullptr, *m_parent_map = nullptr;

        Map *deepest_sub_map();

    public:
        Map() = default;

        ~Map() override;

        explicit Map(Map *parent_map);

        ObjectPtr put(ObjectPtr &object, Map *map) override;

        ObjectPtr &get(const Id &name);

        ObjectPtr end_of_line(Map *map) override;

        void throw_error(const ErrorPtr &error);

        void add_object(const Id &name, const ObjectPtr &object = pending);

        void set_object(const Id &name, const ObjectPtr &object = pending);

        ObjectPtr &get_return() noexcept;

        ObjectPtr &get_temp() noexcept;

        Map *get_parent() noexcept;
    };
}

#endif //PUPS_LIB_TESTS_MAP_H
