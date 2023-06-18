//
// Created by Dogs-Cute on 5/27/2023.
//

#include "numbers.h"

namespace pups::library::builtins::numbers {
    Id id_pass{"", "pass"};
    Id id_add{"", "add"}, id_sub{"", "sub"}, id_mul{"", "mul"}, id_div{"", "div"},
            id_gt{"", "gt"}, id_lt{"", "lt"}, id_ge{"", "ge"}, id_le{"", "le"}, id_eq{"", "eq"}, id_ne{"", "ne"},
            id_and{"", "and"}, id_or{"", "or"};

    ObjectPtr True = std::make_shared<NumType<bool>>(true), False = std::make_shared<NumType<bool>>(false);

    void init(Constants &constants) {
        constants.add(id_pass, False);
    }

}