//
// Created by Dogs-Cute on 5/2/2023.
//

#ifndef POURUPSCRIPTINTERP_LIB_RANDOM_HPP
#define POURUPSCRIPTINTERP_LIB_RANDOM_HPP

#include <random>
#include "../pups.h"
#include "lib_math.hpp"

namespace PUPS {
    static std::random_device random;
    using RandomNumber = decltype(random());

    template<typename Arith>
    typename std::enable_if<std::is_integral<Arith>::value, Arith>::type
    generate(Arith min, Arith max) {
        if (min > max) swap(min, max);
        Arith sub = max - min;
        if (sub > std::random_device::max()) {
            Arith result = 0;
            for (int i = 0; i < sizeof(Arith); ++i) {
                result <<= 8;
                result |= random() % 256;
            }
            return result % sub + min;
        } else
            return static_cast<Arith>(random()) % sub + min;
    }

    template<typename Arith>
    typename std::enable_if<std::is_floating_point<Arith>::value, Arith>::type
    generate(Arith min, Arith max) {
        if (min > max) swap(min, max);
        Arith sub = max - min;
        if (sub > std::random_device::max()) {
            Arith result = 0;
            for (int i = 0; i < sizeof(Arith); ++i) {
                result *= 256;
                result += random() % 256;
            }
            return result / std::random_device::max() * sub + min;
        } else
            return static_cast<Arith>(random()) / std::random_device::max() * sub + min;
    }

#define GET_MIN_MAX(type, access) type min, max; bool is_max = false; \
while (!results.empty()) {                                            \
    if (is_max) max = results.front().result.access;                  \
    else {min = results.front().result.access; is_max = true;}         \
    results.pop();                                                    \
}

    template<typename Arith>
    ObjectPtr generate_object(ResultQueue &results);

    template<>
    ObjectPtr generate_object<long long>(ResultQueue &results) {
        GET_MIN_MAX(long long, r_int)
        return std::make_shared<INST_Int>(TypeCodes::Int, generate(min, max));
    }

    template<>
    ObjectPtr generate_object<double>(ResultQueue &results) {
        GET_MIN_MAX(double, r_float)
        return std::make_shared<INST_Float>(TypeCodes::Float, generate(min, max));
    }

    template<>
    ObjectPtr generate_object<unsigned char>(ResultQueue &results) {
        GET_MIN_MAX(unsigned char, r_byte)
        return std::make_shared<INST_Byte>(TypeCodes::Byte, generate(min, max));
    }

    template<>
    ObjectPtr generate_object<signed char>(ResultQueue &results) {
        GET_MIN_MAX(signed char, r_schar)
        return std::make_shared<INST_Byte>(TypeCodes::SChar, generate(min, max));
    }

    MAKE_BUILTIN(RANDOM_randrange) {
        if (args.size() != 2)
            report.report("RANDOM_randrange", "Two args representing the range should be received.");
        MATH_TYPE_CHECK("RANDOM_randrange")

        MATH_SWITCH_TYPE(generate_object, "RANDOM_randrange")
    }

    void INCLUDE_RANDOM() {
        add_to_builtins("RANDOM_randrange", RANDOM_randrange);
    }
}

#endif //POURUPSCRIPTINTERP_LIB_RANDOM_HPP
