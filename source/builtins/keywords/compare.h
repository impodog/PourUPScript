//
// Created by impodog on 7/30/2023.
//

#ifndef PUPS_LIB_COMPARE_H
#define PUPS_LIB_COMPARE_H

#include "../function.h"
#include "../types/numbers.h"

namespace pups::library::builtins::compare {
    using namespace function;

    struct Lower {
        Map *map;

        explicit Lower(Map *map);

        bool operator()(const ObjectPtr &lhs, ObjectPtr rhs) const;
    };

    struct LowerEqual {
        Map *map;

        explicit LowerEqual(Map *map);

        bool operator()(const ObjectPtr &lhs, ObjectPtr rhs) const;
    };

    struct Greater {
        Map *map;

        explicit Greater(Map *map);

        bool operator()(const ObjectPtr &lhs, ObjectPtr rhs) const;
    };

    struct GreaterEqual {
        Map *map;

        explicit GreaterEqual(Map *map);

        bool operator()(const ObjectPtr &lhs, ObjectPtr rhs) const;
    };

    struct Equal {
        Map *map;

        explicit Equal(Map *map);

        bool operator()(const ObjectPtr &lhs, ObjectPtr rhs) const;
    };

    struct NotEqual {
        Map *map;

        explicit NotEqual(Map *map);

        bool operator()(const ObjectPtr &lhs, ObjectPtr rhs) const;
    };

    using CompareFunc = std::function<bool(ObjectPtr, ObjectPtr)>;

    CompareFunc get_compare(const std::shared_ptr<Function> &func, Map *map);

    void init(Constants &constants);
}

#endif //PUPS_LIB_COMPARE_H
