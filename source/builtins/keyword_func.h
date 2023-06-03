//
// Created by Dogs-Cute on 5/28/2023.
//

#ifndef PUPS_LIB_TESTS_KEYWORD_FUNC_H
#define PUPS_LIB_TESTS_KEYWORD_FUNC_H

#include "function.h"

namespace pups::library::builtins::keyword_func {
    using namespace function;

    class Print : public Function {
    public:
        Print();
    };

    class MoveTo : public Function {
    public:
        MoveTo();
    };

    class If : public Function {
    public:
        explicit If(bool require_false);
    };

    class While : public Function {
    public:
        explicit While(bool require_false);
    };

    class Return : public Function {
    public:
        Return();
    };

    class Targeting : public Function {
    public:
        Targeting();
    };

    class CopyOut : public Function {
    public:
        CopyOut();
    };

    void init(Constants &constants);
}

#endif //PUPS_LIB_TESTS_KEYWORD_FUNC_H
