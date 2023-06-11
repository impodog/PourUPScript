//
// Created by Dogs-Cute on 5/27/2023.
//

#ifndef PUPS_LIB_TESTS_NUMBERS_H
#define PUPS_LIB_TESTS_NUMBERS_H

#include "function.h"

namespace pups::library::builtins::numbers {
    using namespace function;

#define is_type(ty) std::is_same<Arithmetic, ty>::value

    template<typename Arithmetic>
    constexpr const char *type_name_of_arith() noexcept {
        if constexpr (is_type(int))
            return "int";
        else if constexpr (is_type(float))
            return "float";
        else if constexpr (is_type(bool))
            return "bool";
        else
            static_assert(is_type(int), "Type is not supported");
    }

#undef is_type

    template<typename Arithmetic>
    class Number : public Object {
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
            return std::to_string(value);
        }

        [[nodiscard]] bool condition() const noexcept override {
            if constexpr (std::is_same<Arithmetic, bool>::value)
                return value;
            else
                return value != 0;
        }
    };

    template<typename Arithmetic>
    struct NumTypes {
        using NumberType = Number<Arithmetic>;
        using NumberPtr = std::shared_ptr<NumberType>;
        using OperatorCore = std::function<ObjectPtr(const NumberPtr &, const NumberPtr &)>;
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

    extern ObjectPtr True, False;

    void init(Constants &constants);
}
#endif //PUPS_LIB_TESTS_NUMBERS_H
