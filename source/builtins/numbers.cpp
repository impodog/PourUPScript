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

    template<typename Arithmetic>
    FunctionCore from_num_core(const NumCore<Arithmetic> &core) {
        return [core](FunctionArgs &args, Map *map) -> ObjectPtr {
            if (args.size() != 2)
                map->throw_error(std::make_shared<ArgumentError>("Number operator requires two arguments."));
            else {
                auto first = std::dynamic_pointer_cast<NumType<Arithmetic>>(*args.front());
                args.pop();
                auto second = std::dynamic_pointer_cast<NumType<Arithmetic>>(*args.front());
                args.pop();
                if (first && second) {
                    return core(first, second);
                }
                map->throw_error(std::make_shared<TypeError>("Number operator cannot process due to type error."));
            }
            return pending;
        };
    }

    template<typename Arithmetic>
    void add_num_func(Constants &constants, const std::string &name, const NumCore<Arithmetic> &core) {
        constants.add(template_name(name, {type_name_of_arith<Arithmetic>()}),
                      std::make_shared<Function>(from_num_core<Arithmetic>(core)));
    }

#define OP_FUNC(name, op) template<typename Arithmetic, typename return_type> \
ObjectPtr name(NumPtrRef<Arithmetic> lhs, NumPtrRef<Arithmetic> rhs) {\
    return std::make_shared<NumType<return_type>>(static_cast<return_type>(lhs->value op rhs->value));\
}
#define BIN_FUNC(name, bin) template<typename Arithmetic, typename return_type> \
ObjectPtr name(NumPtrRef<Arithmetic> lhs, NumPtrRef<Arithmetic> rhs) {\
    return std::make_shared<NumType<return_type>>(static_cast<return_type>(bin(lhs->value, rhs->value)));\
}

    OP_FUNC(op_add, +)

    OP_FUNC(op_sub, -)

    OP_FUNC(op_mul, *)

    OP_FUNC(op_div, /)

    BIN_FUNC(op_pow, pow)

    template<typename Arithmetic>
    Arithmetic log(Arithmetic x, Arithmetic y) {
        return log2(y) / log2(x);
    }

    BIN_FUNC(op_log, log)

    OP_FUNC(op_and, &)

    OP_FUNC(op_or, |)

    OP_FUNC(op_xor, ^)

    OP_FUNC(op_gt, >)

    OP_FUNC(op_ge, >=)

    OP_FUNC(op_lt, <)

    OP_FUNC(op_le, <=)

    OP_FUNC(op_eq, ==)

    OP_FUNC(op_ne, !=)

#undef OP_FUNC

#define ADD_FUNC(ty, ret, name, op) add_num_func<ty>(constants, name, op<ty, ret>);
#define INT_T_FUNC(name, ret, op) ADD_FUNC(int,ret,name,op)
#define FLOAT_T_FUNC(name, ret, op) ADD_FUNC(float,ret,name,op)
#define INT_B_FUNC(name, op) INT_T_FUNC(name, bool, op)
#define FLOAT_B_FUNC(name, op) FLOAT_T_FUNC(name, bool, op)
#define INT_FUNC(name, op) ADD_FUNC(int,int,name,op)
#define FLOAT_FUNC(name, op) ADD_FUNC(float,float,name,op)
#define INT_FLOAT_FUNC(name, op) INT_FUNC(name, op) FLOAT_FUNC(name, op)
#define INT_FLOAT_B_FUNC(name, op) INT_B_FUNC(name, op) FLOAT_B_FUNC(name, op)

    void init(Constants &constants) {
        constants.add(id_pass, False);
        INT_FLOAT_FUNC("add", op_add)
        INT_FLOAT_FUNC("sub", op_sub)
        INT_FLOAT_FUNC("mul", op_mul)
        INT_FLOAT_FUNC("div", op_div)
        INT_FLOAT_FUNC("pow", op_pow)
        INT_FLOAT_FUNC("log", op_log)
        INT_FUNC("and", op_and)
        INT_FUNC("or", op_or)
        INT_FUNC("xor", op_xor)
        INT_FLOAT_B_FUNC("gt", op_gt)
        INT_FLOAT_B_FUNC("ge", op_ge)
        INT_FLOAT_B_FUNC("lt", op_lt)
        INT_FLOAT_B_FUNC("le", op_le)
        INT_FLOAT_B_FUNC("eq", op_eq)
        INT_FLOAT_B_FUNC("ne", op_ne)
    }

#undef ADD_FUNC
#undef INT_T_FUNC
#undef FLOAT_T_FUNC
#undef INT_B_FUNC
#undef FLOAT_B_FUNC
#undef INT_FUNC
#undef FLOAT_FUNC
#undef INT_FLOAT_FUNC
#undef INT_FLOAT_B_FUNC

}