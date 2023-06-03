//
// Created by Dogs-Cute on 5/27/2023.
//

#include "numbers.h"

namespace pups::library::builtins::numbers {
#define OPERATOR(type, op) [](NumPtrRef<type> lhs, NumPtrRef<type> rhs) -> NumPtr<type> {\
                    return std::make_shared<NumType<type>>(lhs->value op rhs->value);\
                }
    Id id_pass{"", "pass"};
    Id id_add{"", "add"}, id_sub{"", "sub"}, id_mul{"", "mul"}, id_div{"", "div"};

    void init(Constants &constants) {
        constants.add(id_pass, std::make_shared<NumType<bool>>(false));
        constants.add(id_add, std::make_shared<Number_Operator>(
                OPERATOR(int, +),
                OPERATOR(float, +)
        ));
        constants.add(id_sub, std::make_shared<Number_Operator>(
                OPERATOR(int, -),
                OPERATOR(float, -)
        ));
        constants.add(id_mul, std::make_shared<Number_Operator>(
                OPERATOR(int, *),
                OPERATOR(float, *)
        ));
        constants.add(id_div, std::make_shared<Number_Operator>(
                OPERATOR(int, /),
                OPERATOR(float, /)
        ));
    }

#undef OPERATOR
}