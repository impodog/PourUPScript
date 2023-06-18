//
// Created by Dogs-Cute on 5/27/2023.
//

#ifndef PUPS_LIB_TESTS_FUNCTION_H
#define PUPS_LIB_TESTS_FUNCTION_H

#include "../control.h"

namespace pups::library::builtins::function {
    using FunctionArgs = std::queue<ObjectPtr *>;
    using FunctionParams = std::vector<Id>;
    using FunctionCore = std::function<ObjectPtr(FunctionArgs &, Map *)>;

    class Function : public Object {
        std::unordered_map<Map *, FunctionArgs> m_args;
        FunctionCore m_core;
    public:
        explicit Function(FunctionCore core);

        ObjectPtr put(ObjectPtr &object, Map *map) override;

        ObjectPtr end_of_line(Map *map) override;

        std::string type_name() const noexcept override;
    };

    class HasMethods : public Object {
    protected:
        ObjectMap used_functions;

    public:
        ObjectPtr &find(const Id &name, Map *map) override;

        virtual FunctionCore get_method(const Id &name) = 0;
    };
}

#endif //PUPS_LIB_TESTS_FUNCTION_H
