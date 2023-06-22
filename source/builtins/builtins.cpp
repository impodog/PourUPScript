//
// Created by Dogs-Cute on 5/27/2023.
//
#include "builtins.h"

namespace pups::library::builtins {
    Id id_true{"", "true"}, id_false{"", "false"}, id_pending{"", "pending"};

    void init(Constants &constants) {
        constants.add(id_true, numbers::True);
        constants.add(id_false, numbers::False);
        constants.add(id_pending, pending);
        numbers::init(constants);
        inline_func::init(constants);
        keyword_func::init(constants);
        strings::init(constants);
        map_open::init(constants);
        builtin_func::init(constants);
        reference::init(constants);
        containers::init(constants);
        ranges::init(constants);
    }
}