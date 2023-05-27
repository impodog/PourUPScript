//
// Created by Dogs-Cute on 5/27/2023.
//

#ifndef PUPS_LIB_TESTS_MAP_H
#define PUPS_LIB_TESTS_MAP_H

#include "error.h"

namespace pups::library {
    using ObjectMap = unordered_map<Id, ObjectPtr, Id::hash>;

    class Map : public Object {
    protected:
        std::stack<ErrorPtr> m_errors;
        ObjectMap m_map;
        ObjectPtr m_base;
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
    };
}

#endif //PUPS_LIB_TESTS_MAP_H
