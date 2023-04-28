//
// Created by Dogs-Cute on 4/22/2023.
//

#ifndef POURUPSCRIPTTEST_STDTYPE_HPP
#define POURUPSCRIPTTEST_STDTYPE_HPP

#include "scope.hpp"

#define INT "int"
#define FLOAT "flo"

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

        void chk_type(const ObjectPtr &object, Report &report) {
            if (object->type() != type())
                report.report(Report_TypeErr,
                              "Type of " + object->to_string() + "is not the same as " + to_string() + ".");
        }

    public:
        Cnt _type;

        explicit InstanceBase(Cnt type) : _type(type) {

        }

        [[nodiscard]] Cnt type() const noexcept override {
            return _type;
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
                        break;
                    }
                } else if (arg.is_symbol())
                    symbol = arg.front();
                else
                    report.report(Report_WrongToken,
                                  "Cannot put a name \"" + std::string(arg) + "\"without any operator . Item skipped.");
                args.pop();
            }
            Compound compound(imme, args);
            return scope->run_compound(compound);
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
            if (token.is_symbol()) {
                if (token.front() == '=') {
                    next_is_value = 1;
                } else
                    report.report(Report_WrongToken,
                                  R"(Types should only get "=" as assign parameter. Item ")" + std::string(token) +
                                  "\" skipped.");
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

#define EVAL_WHEN(t...)template<typename Arith>\
typename std::enable_if<std::is_same<Arith, t>::value, Arith>::type eval(const Token &token)

    EVAL_WHEN(int) {
        return std::stoi(token.str());
    }

    EVAL_WHEN(long) {
        return std::stol(token.str());
    }

    EVAL_WHEN(long long) {
        return std::stoll(token.str());
    }

    EVAL_WHEN(float) {
        return std::stof(token.str());
    }

    EVAL_WHEN(double) {
        return std::stod(token.str());
    }

    EVAL_WHEN(long double) {
        return std::stold(token.str());
    }

    EVAL_WHEN(unsigned long) {
        return std::stoul(token.str());
    }

    EVAL_WHEN(unsigned long long) {
        return std::stoull(token.str());
    }

#undef EVAL_WHEN
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
    };

    template<typename Arith>
    class TP_Arith final : public TypeBase {
    protected:
        ObjectPtr construct(Scope *scope, Report &report) override {
            if (args.size() != 1) {
                report.report(Report_IncorrectArguments,
                              "Only one assign argument is allowed in Arith initialization. Statement skipped.");
                return nullptr;
            }
            auto new_obj = ObjectPtr{new INST_Arith<Arith>(cnt, eval<Arith>(args.front()))};
            while (!names.empty()) {
                scope->set_object(names.front(), new_obj);
                names.pop();
            }
            args.pop();
            return new_obj;
        }
    };

    using INST_Int = INST_Arith<long long>;
    using TP_Int = TP_Arith<long long>;

    using INST_Float = INST_Arith<double>;
    using TP_Float = TP_Arith<double>;
}

#endif //POURUPSCRIPTTEST_STDTYPE_HPP
