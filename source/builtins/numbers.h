//
// Created by Dogs-Cute on 5/27/2023.
//

#ifndef PUPS_LIB_TESTS_NUMBERS_H
#define PUPS_LIB_TESTS_NUMBERS_H

#include "function.h"

namespace pups::library::builtins::numbers {
    using namespace function;

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

        [[nodiscard]] std::string str() const noexcept override {
            return std::to_string(value);
        }

        [[nodiscard]] bool condition() const noexcept override {
            return value != 0;
        }
    };

    template<typename Arithmetic>
    struct NumTypes {
        using NumberType = Number<Arithmetic>;
        using NumberPtr = std::shared_ptr<NumberType>;
        using OperatorCore = std::function<NumberPtr(const NumberPtr &, const NumberPtr &)>;
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

    class Number_Operator : public Function {
    public:
#define CORE_ARG(type) const NumCore<type> &type##_core
#define TRY_TYPE(type) {\
auto lhs = std::dynamic_pointer_cast<NumType<type>>(first),\
rhs = std::dynamic_pointer_cast<NumType<type>>(second);\
if (lhs && rhs)\
    return type##_core(lhs, rhs);\
}

        Number_Operator(CORE_ARG(int), CORE_ARG(float)) :
                Function([int_core, float_core](FunctionArgs &args,
                                                Map *map) -> ObjectPtr { // Here I use copy catch to avoid external deletions
                    if (args.size() != 2)
                        map->throw_error(std::make_shared<ArgumentError>("Number operator requires two arguments."));
                    else {
                        auto &first = *args.front();
                        args.pop();
                        auto &second = *args.front();
                        args.pop();
                        {
                            auto lhs = std::dynamic_pointer_cast<NumType<int>>(
                                    first), rhs = std::dynamic_pointer_cast<NumType<int>>(second);
                            if (lhs && rhs)return int_core(lhs, rhs);
                        }
                        TRY_TYPE(float)
                        map->throw_error(std::make_shared<TypeError>("Number operator cannot process the two types."));
                    }
                    return pending;
                }) {}
    };

#undef CORE_ARG
#undef TRY_TYPE

    void init(Constants &constants);
}
#endif //PUPS_LIB_TESTS_NUMBERS_H
