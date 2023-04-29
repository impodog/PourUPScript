//
// Created by Dogs-Cute on 4/15/2023.
//

#ifndef POURUPSCRIPT_MATH_HPP
#define POURUPSCRIPT_MATH_HPP

#include <algorithm>
#include <cmath>

#define max_of(x) std::numeric_limits<x>::max()
#define min_of(x) std::numeric_limits<x>::min()
#define epsilon_of(x) std::numeric_limits<x>::epsilon()
#define lim_of(x, lim) std::numeric_limits<x>::lim()

namespace PUPS {

#define SIGNED(ret_tp, spec...) template<typename Arith> spec typename std::enable_if<std::is_signed<Arith>::value,ret_tp>::type
#define UNSIGNED(ret_tp, spec...) template<typename Arith> spec typename std::enable_if<std::is_unsigned<Arith>::value,ret_tp>::type
#define NUMBER(ret_tp, spec...) template<typename Arith> spec typename std::enable_if<std::is_arithmetic<Arith>::value,ret_tp>::type
#define sign(arith) sign_of(arith)
#define nsign(arith) (-sign_of(arith))
#define p1 (signed char)1
#define n1 (signed char)(-1)

    template<typename NumType>
    constexpr typename std::enable_if<std::is_integral<NumType>::value, bool>::type
    eq0(NumType x) {
        return x == 0;
    }

    template<typename NumType>
    constexpr typename std::enable_if<std::is_floating_point<NumType>::value, bool>::type
    eq0(NumType x) {
        return abs(x) <= epsilon_of(NumType);
    }

    template<typename NumType>
    inline constexpr typename std::enable_if<std::is_integral<NumType>::value, NumType>::type
    to_times(NumType x, NumType base) noexcept {
        return x - x % base;
    }

    template<typename NumType>
    inline constexpr typename std::enable_if<std::is_floating_point<NumType>::value, NumType>::type
    to_times(NumType x, NumType base) noexcept {
        return static_cast<long long>(x / base) * base;
    }

    template<typename NumType>
    inline constexpr typename std::enable_if<std::is_integral<NumType>::value, NumType>::type
    to_times(NumType x, NumType add, NumType base) noexcept {
        return x - x % base + add * base;
    }

    template<typename NumType>
    inline constexpr typename std::enable_if<std::is_floating_point<NumType>::value, NumType>::type
    to_times(NumType x, long long add, NumType base) noexcept {
        return (static_cast<long long>(x / base) + add) * base;
    }

    template<typename NumType>
    inline constexpr typename std::enable_if<std::is_integral<NumType>::value, NumType>::type
    pow(NumType x, typename std::make_unsigned<NumType>::type p) {
        NumType result = x;
        for (int i = 1; i < p; i++)
            result *= x;
        return result;
    }

    SIGNED(Arith, constexpr) sign_of(Arith arith) {
        return arith > 0 ? 1 : arith < 0 ? -1 : 0;
    }

    UNSIGNED(Arith, constexpr) sign_of(Arith arith) {
        return arith > 0 ? 1 : 0;
    }

    NUMBER(Arith) sign_if(bool positive) {
        return positive ? 1 : -1;
    }

    NUMBER(Arith) sign_if(bool positive, bool is_0) {
        return is_0 ? 0 : positive ? 1 : -1;
    }

    SIGNED(signed char, constexpr) sign_of_sc(Arith arith) {
        return arith > 0 ? 1 : arith < 0 ? -1 : 0;
    }

    UNSIGNED(signed char, constexpr) sign_of_sc(Arith arith) {
        return arith > 0 ? 1 : 0;
    }

    signed char sign_if_sc(bool positive) {
        return positive ? 1 : -1;
    }

    SIGNED(Arith, constexpr) sign_mov(Arith sign, Arith value) {
        return sign > 0 ? value : sign < 0 ? -value : 0;
    }

    UNSIGNED(Arith, constexpr) sign_mov(Arith sign, Arith value) {
        return sign > 0 ? value : 0;
    }

    SIGNED(Arith) sign_mov(std::initializer_list<Arith> signs, Arith value) {
        bool neg = false;
        for (auto sign: signs)
            switch (sign_of<Arith>(sign)) {
                case 1:
                    break;
                case 0:
                    return 0;
                case -1:
                    neg = !neg;
                    break;
            }
        return neg ? -value : value;
    }

    UNSIGNED(Arith, constexpr) sign_mov(std::initializer_list<Arith> signs, Arith value) {
        return std::any_of(signs.begin(), signs.end(), eq0) ? 0 : value;
    }

    NUMBER(Arith, constexpr) sign_if_mov(bool positive, Arith arith) {
        return positive ? arith : -arith;
    }

    NUMBER(Arith, constexpr) sign_if_mov(bool positive, bool is_0, Arith arith) {
        return is_0 ? 0 : positive ? arith : -arith;
    }


#undef SIGNED
#undef UNSIGNED
#undef p1
#undef n1

    template<bool _canBeNeg = true>
    typename std::enable_if<_canBeNeg, double>::type
    round(double rad) {
        return rad - to_times(rad, 1, M_PI);
    }

    template<bool _canBeNeg = true>
    typename std::enable_if<!_canBeNeg, double>::type
    round(double rad) {
        return rad - to_times(rad, M_PI);
    }

    constexpr double radians(double x) noexcept {
        return x * M_PI / 180;
    }

    constexpr double degrees(double x) noexcept {
        return x * 180 / M_PI;
    }

    template<typename NumType>
    struct Ok {
        NumType value;
        bool ok;

        constexpr operator bool() const noexcept { // NOLINT(google-explicit-constructor)
            return ok;
        }
    };

    template<typename NumType>
    typename std::enable_if<std::is_unsigned<NumType>::value, Ok<NumType>>::type
    add_ok(NumType a, NumType b) noexcept {
        NumType sum = a + b;
        return {sum, (sum > a) && (sum > b)};
    }

    template<typename NumType>
    typename std::enable_if<std::is_signed<NumType>::value, Ok<NumType>>::type
    add_ok(NumType a, NumType b) noexcept {
        NumType sign_a = sign_of(a), sign_b = sign_of(b);
        NumType sum = a + b;
        return {sum, (sign_a != sign_b) || (sign_a == sign_of(sum))};
    }

    template<typename NumType>
    typename std::enable_if<std::is_unsigned<NumType>::value, Ok<NumType>>::type
    sub_ok(NumType a, NumType b) noexcept {
        return {NumType(a - b), a > b};
    }

    template<typename NumType>
    typename std::enable_if<std::is_signed<NumType>::value, Ok<NumType>>::type
    sub_ok(NumType a, NumType b) noexcept {
        return add_ok(a, -b);
    }

#define EVAL_WHEN(fn, t...)template<typename Arith>\
typename std::enable_if<std::is_same<Arith, t>::value, Arith>::type eval(const std::string &s){\
    return std::fn(s);\
}

    EVAL_WHEN(stoi, int)

    EVAL_WHEN(stol, long)

    EVAL_WHEN(stoll, long long)

    EVAL_WHEN(stoul, unsigned long)

    EVAL_WHEN(stoull, unsigned long long)

    EVAL_WHEN(stod, double)

    EVAL_WHEN(stold, long double)

#undef EVAL_WHEN
}

#undef SIGNED
#undef UNSIGNED

#endif //POURUPSCRIPT_MATH_HPP
