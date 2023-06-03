//
// Created by Dogs-Cute on 5/27/2023.
//
#include "builtins.h"

namespace pups::library::builtins {
    void init(Constants &constants) {
        init_numbers(constants);
        init_inline_func(constants);
    }
}