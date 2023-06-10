//
// Created by Dogs-Cute on 5/27/2023.
//

#include "numbers.h"

namespace pups::library::builtins::numbers {
#define OPERATOR_BASE(type, return_type, op) [](NumPtrRef<type> lhs, NumPtrRef<type> rhs) -> NumPtr<return_type> {\
                    return std::make_shared<NumType<return_type>>(static_cast<return_type>(lhs->value op rhs->value));\
                }
#define OPERATOR(type, op) OPERATOR_BASE(type, type, op)
#define BOOL_OPERATOR(type, op) OPERATOR_BASE(type, bool, op)
#define OPERATORS_BASE(macro, op) macro(int, op), macro(float, op)
#define OPERATORS(op) OPERATORS_BASE(OPERATOR, op)
#define BOOL_OPERATORS(op) OPERATORS_BASE(BOOL_OPERATOR, op)
    Id id_pass{"", "pass"};
    Id id_add{"", "add"}, id_sub{"", "sub"}, id_mul{"", "mul"}, id_div{"", "div"},
            id_gt{"", "gt"}, id_lt{"", "lt"}, id_ge{"", "ge"}, id_le{"", "le"}, id_eq{"", "eq"}, id_ne{"", "ne"},
            id_and{"", "and"}, id_or{"", "or"};

    ObjectPtr True = std::make_shared<NumType<bool>>(true), False = std::make_shared<NumType<bool>>(false);

    void init(Constants &constants) {
        constants.add(id_pass, False);
        constants.add(id_add, std::make_shared<Number_Operator>(
                OPERATORS(+)
        ));
        constants.add(id_sub, std::make_shared<Number_Operator>(
                OPERATORS(-)
        ));
        constants.add(id_mul, std::make_shared<Number_Operator>(
                OPERATORS(*)
        ));
        constants.add(id_div, std::make_shared<Number_Operator>(
                OPERATORS(/)
        ));
        constants.add(id_gt, std::make_shared<Number_Operator>(
                BOOL_OPERATORS(>)
        ));
        constants.add(id_lt, std::make_shared<Number_Operator>(
                BOOL_OPERATORS(<)
        ));
        constants.add(id_ge, std::make_shared<Number_Operator>(
                BOOL_OPERATORS(>=)
        ));
        constants.add(id_le, std::make_shared<Number_Operator>(
                BOOL_OPERATORS(<=)
        ));
        constants.add(id_eq, std::make_shared<Number_Operator>(
                BOOL_OPERATORS(==)
        ));
        constants.add(id_ne, std::make_shared<Number_Operator>(
                BOOL_OPERATORS(!=)
        ));
        constants.add(id_and, std::make_shared<Number_Operator>(
                BOOL_OPERATORS(&&)
        ));
        constants.add(id_or, std::make_shared<Number_Operator>(
                BOOL_OPERATORS(||)
        ));
    }

#undef OPERATORS
#undef OPERATORS_BASE
#undef BOOL_OPERATOR
#undef OPERATOR
#undef OPERATOR_BASE

}