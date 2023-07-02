//
// Created by impodog on 7/1/2023.
//

#ifndef PUPS_LIB_TESTS_INLINE_TYPING_H
#define PUPS_LIB_TESTS_INLINE_TYPING_H

#include "../types/typing.h"

namespace pups::library::builtins::inline_typing {
    using namespace typing;

    class TypeInit : public Function {
    public:
        TypeInit();
    };

    class InstanceInit : public Function {
    public:
        InstanceInit();
    };

    void init(Constants &constants);
}

#endif //PUPS_LIB_TESTS_INLINE_TYPING_H
