//
// Created by Dogs-Cute on 5/28/2023.
//

#ifndef PUPS_LIB_TESTS_INLINE_FUNC_H
#define PUPS_LIB_TESTS_INLINE_FUNC_H

#include "function.h"

namespace pups::library::builtins {
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

        static void add_arguments(MapPtr &map, const FunctionArgs &args);

    public:
        explicit InlineFunc(IdFile idFile);
    };

    class InlineFunc_Init : public Function {
    public:
        InlineFunc_Init();
    };

    void init_inline_func(Constants &constants);
}

#endif //PUPS_LIB_TESTS_INLINE_FUNC_H
