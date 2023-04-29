//
// Created by Dogs-Cute on 4/22/2023.
//

#ifndef POURUPSCRIPTTEST_STDTYPE_HPP
#define POURUPSCRIPTTEST_STDTYPE_HPP

#include "scope.hpp"

#define INT "int"
#define FLOAT "flo"
#define CST "cst"

#define INST_OP(op_name) virtual ObjectPtr op_name(Token nxt, Scope *scope, Report &report) {\
    report.report(Report_WrongToken, "op_name is not supported");                       \
    return null_obj;                                                                    \
}
#define CALL_OP(op_name) op_name(arg, scope, report)

namespace PUPS {
    class InstanceBase : public ObjectBase {
    protected:
        std::queue<Token> args;

        INST_OP(op_add)

        INST_OP(op_sub)

        INST_OP(op_mul)

        INST_OP(op_div)

        void chk_type(const ObjectPtr &object, Report &report) const {
            if (object->type() != type())
                report.report(Report_TypeErr,
                              "Type of " + object->to_string() + " is not the same as " + to_string() + ".");
        }

        virtual ObjectPtr copy() const noexcept = 0;

    public:
        Cnt _type;

        explicit InstanceBase(Cnt type) : _type(type) {

        }

        [[nodiscard]] Cnt type() const noexcept override {
            return _type;
        }

        [[nodiscard]] std::string to_string() const noexcept override {
            return "<Instance" + std::to_string(PtrToUlong(this)) + " Code" + std::to_string(cnt) + " WithType" +
                   std::to_string(_type) + ">";
        }

        void put(const Token &token, Report &report) final {
            args.push(token);
        }

        ObjectPtr ends(Scope *scope, Report &report) override {
            char symbol = 0;
            Token imme;
            while (!args.empty()) {
                auto &arg = args.front();
                if (symbol) {
                    ObjectPtr object;
                    switch (symbol) {
                        case '+':
                            object = CALL_OP(op_add);
                            break;
                        case '-':
                            object = CALL_OP(op_sub);
                            break;
                        case '*':
                            object = CALL_OP(op_mul);
                            break;
                        case '/':
                            object = CALL_OP(op_div);
                            break;
                        default:
                            report.report(Report_WrongToken,
                                          "Unknown operator \"" + std::string(arg) + "\". Item skipped.");
                            break;
                    }
                    if (object) {
                        imme = scope->add_imme(object);
                        args.pop(); // required to pop the operator rvalue
                        break;
                    }
                } else if (arg.is_symbol())
                    symbol = arg.front();
                else
                    report.report(Report_WrongToken,
                                  "Cannot put a name \"" + std::string(arg) +
                                  "\" without any operator . Item skipped.");
                args.pop();
            }
            if (!imme.eof()) {
                Compound compound(imme, args);
                return scope->run_compound(compound);
            }
            return copy();
        }

        void exit(PUPS::Report &report) override {

        }

        [[nodiscard]] bool can_delete() const noexcept override {
            return true;
        }
    };

#undef INST_OP
#undef CALL_OP

    class TypeBase : public ObjectBase {
        signed char next_is_value = 0;
    protected:
        virtual ObjectPtr construct(Scope *scope, Report &report) = 0;

        std::queue<Token> names, args;
    public:
        void put(const Token &token, Report &report) override {
            if (token.is_symbol() && token.front() == '=') {
                next_is_value = 1;
            } else {
                if (next_is_value) {
                    args.push(token);
                    next_is_value = -1;
                } else
                    names.push(token);
            }
        }

        ObjectPtr ends(PUPS::Scope *scope, PUPS::Report &report) override {
            auto result = construct(scope, report);
            if (!args.empty()) {
                report.report(Report_IncorrectArguments,
                              "Arguments are not enough for type initialization. (" + std::to_string(args.size()) +
                              " remaining)");
            }
            next_is_value = 0;
            return ObjectPtr{result};
        }

        void exit(PUPS::Report &report) override {

        }

        [[nodiscard]] bool can_delete() const noexcept override {
            return false;
        }
    };

    class StdTypeBase : public TypeBase {
    protected:
        static void chk_cst(Scope *scope, Report &report) {
            if (!scope->flags.cst)
                report.report(Report_UnConstInit, "Std type initialization with \"" CST
                                                  "\" qualifier is suggested. Statement continued in spite of warnings.");
        }

        ObjectPtr ends(PUPS::Scope *scope, PUPS::Report &report) override {
            chk_cst(scope, report);
            return TypeBase::ends(scope, report);
        }
    };

#define OVERRIDE_OP(op_name, op) ObjectPtr op_name(Token nxt, Scope *scope, Report &report) override {\
    auto &object = scope->find(nxt);\
    chk_type(object, report);\
    return ObjectPtr{new INST_Arith{type(), value op std::static_pointer_cast<INST_Arith>(object)->value}};\
}

    template<typename Arith>
    class INST_Arith final : public InstanceBase {
        Arith value;
        std::queue<Token> args;

    public:
        INST_Arith(Cnt type, Arith value) : InstanceBase(type), value(value) {}

        [[nodiscard]] std::string to_string() const noexcept override {
            return std::to_string(value);
        }

        OVERRIDE_OP(op_add, +)

        OVERRIDE_OP(op_sub, -)

        OVERRIDE_OP(op_mul, *)

        OVERRIDE_OP(op_div, /)

        [[nodiscard]] ObjectPtr copy() const noexcept override {
            return ObjectPtr{new INST_Arith{*this}};
        }
    };

    template<typename Arith>
    class TP_Arith final : public StdTypeBase {
    protected:
        ObjectPtr construct(Scope *scope, Report &report) override {
            ObjectPtr new_obj;
            std::string args_s = join_tokens(args);
            try {
                new_obj = ObjectPtr{
                        new INST_Arith<Arith>(cnt, eval<Arith>(args_s))};
            } catch (const std::invalid_argument &) {
                report.report(Report_IncorrectArguments,
                              "Cannot evaluate arguments \"" + args_s +
                              "\" to arithmetic type. Statement skipped.");
                return null_obj;
            }
            while (!names.empty()) {
                scope->set_object(names.front(), new_obj);
                names.pop();
            }
            return new_obj;
        }
    };

    using INST_Int = INST_Arith<long long>;
    using TP_Int = TP_Arith<long long>;

    using INST_Float = INST_Arith<double>;
    using TP_Float = TP_Arith<double>;
}

#endif //POURUPSCRIPTTEST_STDTYPE_HPP
