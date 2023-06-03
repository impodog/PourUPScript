//
// Created by Dogs-Cute on 5/27/2023.
//

#ifndef PUPS_LIB_TESTS_FUNCTION_H
#define PUPS_LIB_TESTS_FUNCTION_H

#include "../control.h"

namespace pups::library::builtins {
    using FunctionArgs = vector<ObjectPtr *>;
    using FunctionParams = vector<Id>;
    using FunctionCore = function<ObjectPtr(const FunctionArgs &, Map *)>;

    class Function : public Object {
    protected:
        FunctionCore m_core;
        unordered_map<Map *, FunctionArgs> m_args;
    public:
        explicit Function(FunctionCore core);

        ObjectPtr put(ObjectPtr &object, Map *map) override;

        ObjectPtr end_of_line(Map *map) override;
    };
}

#endif //PUPS_LIB_TESTS_FUNCTION_H
