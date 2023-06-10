//
// Created by impodog on 6/4/2023.
//

#ifndef PUPS_LIB_TESTS_BUILTIN_FUNC_H
#define PUPS_LIB_TESTS_BUILTIN_FUNC_H

#include "function.h"

namespace pups::library::builtins::builtin_func {
    using namespace function;

    class Inputs : public Function {
    public:
        Inputs();
    };

    class Println : public Function {
    public:
        Println();
    };

    class Print : public Function {
    public:
        Print();
    };

    void init(Constants &constants);
}

#endif //PUPS_LIB_TESTS_BUILTIN_FUNC_H
