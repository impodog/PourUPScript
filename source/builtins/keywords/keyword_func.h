//
// Created by Dogs-Cute on 5/28/2023.
//

#ifndef PUPS_LIB_KEYWORD_FUNC_H
#define PUPS_LIB_KEYWORD_FUNC_H

#include "../function.h"

namespace pups::library::builtins::keyword_func {
    using namespace function;

    class MoveTo : public Function {
    public:
        MoveTo();
    };

    class IfNoArg : public Function {
    public:
        explicit IfNoArg(bool require_false, bool is_direct);
    };

    class Loop : public Function {
    public:
        explicit Loop(bool require_false);
    };

    class Return : public Function {
    public:
        Return();
    };

    class With : public Function {
    public:
        With();
    };

    class Pop : public Function {
    public:
        Pop();
    };

    class Delete : public Function {
    public:
        Delete();
    };

    class Break : public Function {
    public:
        Break();
    };

    class Unmap : public Function {
    public:
        Unmap();
    };

    class Exit : public Function {
    public:
        Exit();
    };

    class Throw : public Function {
    public:
        Throw();
    };

    void init(Constants &constants);
}

#endif //PUPS_LIB_KEYWORD_FUNC_H
