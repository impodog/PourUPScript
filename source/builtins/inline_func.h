//
// Created by Dogs-Cute on 5/28/2023.
//

#ifndef PUPS_LIB_TESTS_INLINE_FUNC_H
#define PUPS_LIB_TESTS_INLINE_FUNC_H

#include "function.h"

namespace pups::library::builtins::inline_func {
    using namespace function;

    class Arguments final : public Object {
    protected:
        FunctionArgs args;
    public:
        explicit Arguments(FunctionArgs args);

        ObjectPtr put(ObjectPtr &object, Map *map) override;
    };

    class InlineFunc : public Function {
    protected:
        IdFile m_idFile;
        Map *m_static_link;

        static void add_arguments(MapPtr &map, const FunctionArgs &args);

    public:
        InlineFunc(IdFile idFile, Map *m_static_link);
    };

    class InlineFunc_Init : public Function {
    public:
        InlineFunc_Init();
    };

    void init(Constants &constants);
}

#endif //PUPS_LIB_TESTS_INLINE_FUNC_H
