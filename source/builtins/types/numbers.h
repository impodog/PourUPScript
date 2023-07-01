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
        static_assert(std::is_arithmetic<Arithmetic>::value, "Number type must be arithmetic.");
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
            if constexpr (std::is_same<Arithmetic, bool>::value)
                return value ? "true" : "false";
            else
                return std::to_string(value);
        }

        FunctionCore get_method(const pups::library::Id &name) override;

        [[nodiscard]] bool condition() const noexcept override {
            if constexpr (std::is_same<Arithmetic, bool>::value)
                return value;
            else
                return value != 0;
        }

        [[nodiscard]] size_t hash() const noexcept override {
            return std::hash<Arithmetic>()(value);
        }

        [[nodiscard]] size_t equal(const ObjectPtr &object) const noexcept override {
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
        static const OperatorMap operators;
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
    Arithmetic log(Arithmetic x, Arithmetic y) {
        return log2(y) / log2(x);
    }

#define OP_FUNC_TYPED(op, return_type) [](NumType<Arithmetic> &lhs, NumType<Arithmetic> &rhs) -> ObjectPtr {\
    return std::make_shared<NumType<return_type>>(static_cast<return_type>(lhs.value op rhs.value));\
}
#define OP_FUNC(op) OP_FUNC_TYPED(op, Arithmetic)
#define OP_FUNC_CMP(op) OP_FUNC_TYPED(op, bool)
#define BIN_FUNC_TYPED(op, return_type) [](NumType<Arithmetic> &lhs, NumType<Arithmetic> &rhs) -> ObjectPtr {\
    return std::make_shared<NumType<return_type>>(static_cast<return_type>(op(lhs.value, rhs.value)));\
}
#define BIN_FUNC(op) BIN_FUNC_TYPED(op, Arithmetic)
#define BIN_FUNC_CMP(op) BIN_FUNC_TYPED(op, bool)
    template<typename Arithmetic>
    const typename NumTypes<Arithmetic>::OperatorMap NumTypes<Arithmetic>::operators = {
            {Id{"", "add"}, OP_FUNC(+)},
            {Id{"", "sub"}, OP_FUNC(-)},
            {Id{"", "mul"}, OP_FUNC(*)},
            {Id{"", "div"}, OP_FUNC(/)},
            {Id{"", "pow"}, BIN_FUNC(pow)},
            {Id{"", "log"}, BIN_FUNC(log)},
            {Id{"", "gt"},  OP_FUNC(>)},
            {Id{"", "ge"},  OP_FUNC(>=)},
            {Id{"", "lt"},  OP_FUNC(<)},
            {Id{"", "le"},  OP_FUNC(<=)},
            {Id{"", "eq"},  OP_FUNC(==)},
            {Id{"", "ne"},  OP_FUNC(!=)},
    };

    template<typename Arithmetic>
    FunctionCore Number<Arithmetic>::get_method(const Id &name) {
        const auto &func = NumTypes<Arithmetic>::operators.at(name);
        return [this, &func](FunctionArgs &args, Map *map) -> ObjectPtr {
            if (args.size() != 1)
                map->throw_error(std::make_shared<ArgumentError>("Number operator requires one only argument."));
            else {
                auto ptr = std::dynamic_pointer_cast<Number<Arithmetic>>(*args.front());
                if (ptr)
                    return func(*this, *ptr);
                else
                    map->throw_error(
                            std::make_shared<TypeError>("Number operator requires numbers with the same type."));
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
