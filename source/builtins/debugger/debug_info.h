//
// Created by impodog on 7/25/2023.
//

#ifndef PUPS_LIB_TESTS_DEBUG_INFO_H
#define PUPS_LIB_TESTS_DEBUG_INFO_H

#include "../function.h"
#include "../types/strings.h"

namespace pups::library::builtins::debug_info {
    using namespace function;

    class DebugInfo : public Function {
    public:
        DebugInfo();
    };

    void init(Constants &constants);
}

#endif //PUPS_LIB_TESTS_DEBUG_INFO_H
