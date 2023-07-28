//
// Created by Dogs-Cute on 5/27/2023.
//

#ifndef PUPS_LIB_NUMBERS_H
#define PUPS_LIB_NUMBERS_H

#include "../function.h"

namespace pups::library::builtins::numbers {
    using namespace function;

#define is_type(ty) std::is_same<Arithmetic, ty>::value

    template<typename Arithmetic>
    constexpr const char *type_name_of_arith() noexcept {
        if constexpr (is_type(pups_int))
            return "int";
        else if constexpr (is_type(pups_float))
            return "float";
        else if constexpr (is_type(pups_bool))
            return "bool";
        else
            static_assert(is_type(int), "Type is not supported");
    }

#undef is_type

    template<typename Arithmetic>
    class Number : public HasMethods {
        static_assert(std::is_arithmetic_v<Arithmetic>, "Number type must be arithmetic.");

    public:
        Arithmetic value;

        explicit Number(Arithmetic value) : value(value) {}

        ObjectPtr put(ObjectPtr &object, Map *map) override {
            map->throw_error(std::make_shared<TypeError>("Numbers do NOT allow putting."));
            return nullptr;
        }


        [[nodiscard]] std::string type_name() const noexcept override {
            return type_name_of_arith<Arithmetic>();
        }

        [[nodiscard]] std::string str() const noexcept override {
            if constexpr (std::is_same_v<Arithmetic, bool>)
                return value ? "true" : "false";
            else
                return std::to_string(value);
        }

        FunctionCore get_method(const Id &name) override;

        [[nodiscard]] bool condition() const noexcept override {
            if constexpr (std::is_same_v<Arithmetic, bool>)
                return value;
            else
                return value != 0;
        }

        [[nodiscard]] size_t hash() noexcept override {
            return std::hash<Arithmetic>()(value);
        }

        [[nodiscard]] bool equal(ObjectPtr &object) noexcept override {
            auto ptr = cast<Number>(object);
            return ptr && value == ptr->value;
        }
    };

    template<typename Arithmetic>
    struct NumTypes {
        using NumberType = Number<Arithmetic>;
        using NumberPtr = std::shared_ptr<NumberType>;
        using OperatorCore = std::function<ObjectPtr(NumberType &, NumberType &)>;
        using OperatorMap = IdMap<OperatorCore>;
    };

    template<typename Arithmetic>
    using NumType = typename NumTypes<Arithmetic>::NumberType;
    template<typename Arithmetic>
    using NumPtr = typename NumTypes<Arithmetic>::NumberPtr;
    template<typename Arithmetic>
    using NumPtrRef = const typename NumTypes<Arithmetic>::NumberPtr &;
    template<typename Arithmetic>
    using NumCore = typename NumTypes<Arithmetic>::OperatorCore;
    template<typename Arithmetic>
    using NumCoreRef = const NumCore<Arithmetic> &;

    template<typename Arithmetic>
    struct OperatorTypes {
        static const typename NumTypes<Arithmetic>::OperatorMap operators;
    };


    template<typename Arithmetic>
    Arithmetic log(Arithmetic x, Arithmetic y) {
        return static_cast<Arithmetic>(log2(y) / log2(x));
    }

#define OP_FUNC_TYPED(Arithmetic, op, return_type) [](NumType<Arithmetic> &lhs, NumType<Arithmetic> &rhs) -> ObjectPtr {\
    return std::make_shared<NumType<return_type>>(static_cast<return_type>(lhs.value op rhs.value));\
}
#define OP_FUNC(Arithmetic, op) OP_FUNC_TYPED(Arithmetic, op, Arithmetic)
#define OP_FUNC_CMP(Arithmetic, op) OP_FUNC_TYPED(Arithmetic, op, bool)
#define BIN_FUNC_TYPED(Arithmetic, op, return_type) [](NumType<Arithmetic> &lhs, NumType<Arithmetic> &rhs) -> ObjectPtr {\
    return std::make_shared<NumType<return_type>>(static_cast<return_type>(op(lhs.value, rhs.value)));\
}
#define BIN_FUNC(Arithmetic, op) BIN_FUNC_TYPED(Arithmetic, op, Arithmetic)
#define BIN_FUNC_CMP(Arithmetic, op) BIN_FUNC_TYPED(Arithmetic, op, bool)

    template<>
    struct OperatorTypes<pups_int> {
        static const NumTypes<pups_int>::OperatorMap operators;
    };

    inline const NumTypes<pups_int>::OperatorMap OperatorTypes<pups_int>::operators = {
            {Id{"", "add"}, OP_FUNC(pups_int, +)},
            {Id{"", "sub"}, OP_FUNC(pups_int, -)},
            {Id{"", "mul"}, OP_FUNC(pups_int, *)},
            {Id{"", "div"}, OP_FUNC(pups_int, /)},
            {Id{"", "mod"}, OP_FUNC(pups_int, %)},
            {Id{"", "and"}, OP_FUNC(pups_int, &)},
            {Id{"", "or"},  OP_FUNC(pups_int, |)},
            {Id{"", "xor"}, OP_FUNC(pups_int, ^)},
            {Id{"", "pow"}, BIN_FUNC(pups_int, pow)},
            {Id{"", "log"}, BIN_FUNC(pups_int, log)},
            {Id{"", "gt"},  OP_FUNC_CMP(pups_int, >)},
            {Id{"", "ge"},  OP_FUNC_CMP(pups_int, >=)},
            {Id{"", "lt"},  OP_FUNC_CMP(pups_int, <)},
            {Id{"", "le"},  OP_FUNC_CMP(pups_int, <=)},
            {Id{"", "eq"},  OP_FUNC_CMP(pups_int, ==)},
            {Id{"", "ne"},  OP_FUNC_CMP(pups_int, !=)},
    };

    template<>
    struct OperatorTypes<pups_float> {
        static const NumTypes<pups_float>::OperatorMap operators;
    };

    inline const NumTypes<pups_float>::OperatorMap OperatorTypes<pups_float>::operators = {
            {Id{"", "add"}, OP_FUNC(pups_float, +)},
            {Id{"", "sub"}, OP_FUNC(pups_float, -)},
            {Id{"", "mul"}, OP_FUNC(pups_float, *)},
            {Id{"", "div"}, OP_FUNC(pups_float, /)},
            {Id{"", "pow"}, BIN_FUNC(pups_float, pow)},
            {Id{"", "log"}, BIN_FUNC(pups_float, log)},
            {Id{"", "gt"},  OP_FUNC_CMP(pups_float, >)},
            {Id{"", "ge"},  OP_FUNC_CMP(pups_float, >=)},
            {Id{"", "lt"},  OP_FUNC_CMP(pups_float, <)},
            {Id{"", "le"},  OP_FUNC_CMP(pups_float, <=)},
            {Id{"", "eq"},  OP_FUNC_CMP(pups_float, ==)},
            {Id{"", "ne"},  OP_FUNC_CMP(pups_float, !=)},
    };

    template<>
    struct OperatorTypes<pups_bool> {
        static const NumTypes<pups_bool>::OperatorMap operators;
    };

    inline const NumTypes<pups_bool>::OperatorMap OperatorTypes<pups_bool>::operators = {
            {Id{"", "and"}, OP_FUNC(pups_bool, &&)},
            {Id{"", "or"}, OP_FUNC(pups_bool, ||)},
            {Id{"", "xor"}, OP_FUNC(pups_bool, ^)},
    };

    template<typename Arithmetic>
    FunctionCore Number<Arithmetic>::get_method(const Id &name) {
        const auto &func = OperatorTypes<Arithmetic>::operators.at(name);
        return [this, &func](FunctionArgs &args, Map *map) -> ObjectPtr {
            if (args.size() != 1)
                map->throw_error(
                        std::make_shared<ArgumentError>("Number operator requires one only argument."));
            else {
                auto ptr = std::dynamic_pointer_cast<Number<Arithmetic>>(*args.front());
                if (ptr)
                    return func(*this, *ptr);
                map->throw_error(
                        std::make_shared<TypeError>(
                                "Number operator requires numbers with the same type."));
            }
            return pending;
        };
    }

    extern ObjectPtr True, False;

    using IntType = NumType<pups_int>;
    using FloatType = NumType<pups_float>;
    using BoolType = NumType<pups_bool>;

    void init(Constants &constants);
}
#endif //PUPS_LIB_NUMBERS_H
