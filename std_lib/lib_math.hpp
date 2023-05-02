//
// Created by Dogs-Cute on 5/1/2023.
//

#ifndef POURUPSCRIPTINTERP_LIB_MATH_HPP
#define POURUPSCRIPTINTERP_LIB_MATH_HPP

#include "../pups.h"
#include <cmath>

#define MATH_TYPE_CHECK_BASE(err_name, work...) using enum EvalResult::ResultType;\
EvalResult::ResultType type = type_default;\
while (!args.empty()) {\
    auto v = (*args.front())->get_ev();\
    if (type) {\
        if (type != v.type)\
            report.report(err_name, "Incorrect type.");\
    } else type = v.type;\
    work\
    args.pop();\
}
#define MATH_TYPE_CHECK(err_name)  ResultQueue results; MATH_TYPE_CHECK_BASE(err_name, results.push(v);)
#define MATH_SWITCH_TYPE_BASE(func, err_name, results...) switch (type) {\
    case EvalResult::type_int:\
        return func<long long>(results);\
    case EvalResult::type_float:\
        return func<double>(results);\
    case EvalResult::type_byte:\
        return func<unsigned char>(results);\
    case EvalResult::type_schar:\
        return func<signed char>(results);\
    default:\
        report.report(err_name, "Incorrect type.");\
        return null_obj;\
}
#define MATH_SWITCH_TYPE(func, err_name) MATH_SWITCH_TYPE_BASE(func, err_name, results)

#define MATH_MAKE_FUNCTIONS(generate) generate(long long, r_int)\
generate(double, r_float)\
generate(unsigned char, r_byte)\
generate(signed char, r_schar)

#define MATH_MAKE_FUNCTIONS_TYPED(generate) generate(long long, r_int, INST_Int, TypeCodes::Int)\
generate(double, r_float, INST_Float, TypeCodes::Float)\
generate(unsigned char, r_byte, INST_Byte, TypeCodes::Byte)\
generate(signed char, r_schar, INST_SChar, TypeCodes::SChar)

namespace PUPS {
    using ResultQueue = std::queue<EvalResult>;

    template<typename T>
    static ObjectPtr MATH_sqrt_base(ResultQueue &results);

#define SQRT_FUNC(type, access) template<>                     \
inline ObjectPtr MATH_sqrt_base<type>(ResultQueue &results) {         \
    double res = 0;                                            \
    while (!results.empty()) {                                 \
        res += ::sqrt(results.front().result.access);          \
        results.pop();                                         \
    }                                                          \
    return std::make_shared<INST_Float>(TypeCodes::Float, res);\
}

    MATH_MAKE_FUNCTIONS(SQRT_FUNC)

#undef SQRT_FUNC

    MAKE_BUILTIN(MATH_sqrt) {
        MATH_TYPE_CHECK("MATH_sqrt")
        MATH_SWITCH_TYPE(MATH_sqrt_base, "MATH_sqrt")
    }

    template<typename T>
    static ObjectPtr MATH_pow_base(ResultQueue &results);

#define POW_FUNC(type, access, type_ret, typecode) template<>          \
inline ObjectPtr MATH_pow_base<type>(ResultQueue &results) {                  \
    type base = 0, power = 0;                                          \
    bool status = false;                                               \
    while (!results.empty()) {                                         \
        if (status) {                                                  \
            power = results.front().result.access;                     \
        } else {                                                       \
            base = results.front().result.access;                      \
            status = true;                                             \
        }                                                              \
        results.pop();                                                 \
    }                                                                  \
    return std::make_shared<type_ret>(typecode, std::pow(base, power));\
}

    MATH_MAKE_FUNCTIONS_TYPED(POW_FUNC)

#undef POW_FUNC

    MAKE_BUILTIN(MATH_pow) {
        MATH_TYPE_CHECK("MATH_pow")
        MATH_SWITCH_TYPE(MATH_pow_base, "MATH_pow")
    }

    inline void INCLUDE_MATH() {
        add_to_builtins("MATH_sqrt", MATH_sqrt);
        add_to_builtins("MATH_pow", MATH_pow);
    }
}

#undef MATH_MAKE_FUNCTIONS
#undef MATH_MAKE_FUNCTIONS_TYPED

#endif //POURUPSCRIPTINTERP_LIB_MATH_HPP
