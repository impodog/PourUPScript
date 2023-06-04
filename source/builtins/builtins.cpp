//
// Created by Dogs-Cute on 5/27/2023.
//
#include "builtins.h"

namespace pups::library::builtins {
    void init(Constants &constants) {
        numbers::init(constants);
        inline_func::init(constants);
        keyword_func::init(constants);
        strings::init(constants);
        map_open::init(constants);
        builtin_func::init(constants);
    }
}