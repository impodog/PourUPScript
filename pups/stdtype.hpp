//
// Created by Dogs-Cute on 4/22/2023.
//

#ifndef POURUPSCRIPTTEST_STDTYPE_HPP
#define POURUPSCRIPTTEST_STDTYPE_HPP

#include <utility>

#include "scope.hpp"

#define INST_OP(op_name) virtual ObjectPtr op_name(const Token &nxt, Scope *scope, Report &report) {\
    report.report(Report_WrongToken, "operator is not supported");                       \
    return null_obj;                                                                    \
}
#define CALL_OP(op_name) object = op_name(top.front(), scope, report);

namespace PUPS {
    class InstanceBase : public ObjectBase {
        std::stack<std::queue<Token>> args;
    protected:

        INST_OP(op_add)

        INST_OP(op_sub)

        INST_OP(op_mul)

        INST_OP(op_div)

        INST_OP(op_lt)

        INST_OP(op_gt)

        INST_OP(op_and)

        INST_OP(op_or)

        INST_OP(op_xor)

        INST_OP(op_dot)

        INST_OP(op_eq)

        void chk_type_same(const ObjectPtr &object, Report &report) const {
            if (object->type() != type())
                report.report(Report_TypeErr,
                              "Type of " + object->to_string() + " is not the same as " + to_string() + ".");
        }

        [[nodiscard]] constexpr bool is_instance() const noexcept final {
            return true;
        }

        virtual void inst_put(const Token &token, Report &report) {
            report.report(Report_IncorrectArguments,
                          "This instance does not allow keyword-like calling. \"" + std::string(token) + "\" skipped.");
        }

        virtual ObjectPtr inst_ends(Scope *scope, Report &report) {
            return copy();
        }

        [[nodiscard]] virtual ObjectPtr copy() const noexcept = 0;

    public:
        Cnt _type;

        explicit InstanceBase(Cnt type) : _type(type) {
            args.emplace();
        }

        [[nodiscard]] Cnt type() const noexcept override {
            return _type;
        }

        [[nodiscard]] std::string to_string() const noexcept override {
            return "<Instance" + std::to_string(PtrToUlong(this)) + " Code" + std::to_string(cnt) + " WithType" +
                   std::to_string(_type) + ">";
        }

        void put(const Token &token, Report &report) final {
            args.top().push(token);
        }

        ObjectPtr ends(Scope *scope, Report &report) final {
            char symbol = 0;
            Token imme;
            auto &top = args.top();
            args.emplace();
            if (top.empty()) symbol = call_symbol;
            while (!top.empty()) {
                if (symbol == call_symbol)
                    inst_put:
                    inst_put(top.front(), report);
                else if (symbol) {
                    ObjectPtr object;
                    switch (symbol) {
                        case '+':
                            CALL_OP(op_add)
                            break;
                        case '-':
                            CALL_OP(op_sub)
                            break;
                        case '*':
                            CALL_OP(op_mul)
                            break;
                        case '/':
                            CALL_OP(op_div)
                            break;
                        case '<':
                            CALL_OP(op_lt)
                            break;
                        case '>':
                            CALL_OP(op_gt)
                            break;
                        case '&':
                            CALL_OP(op_and)
                            break;
                        case '|':
                            CALL_OP(op_or)
                            break;
                        case '^':
                            CALL_OP(op_xor)
                            break;
                        case '.':
                            CALL_OP(op_dot)
                            break;
                        case '=':
                            CALL_OP(op_eq)
                            break;
                        default:
                            report.report(Report_WrongToken,
                                          "Unknown operator \"" + std::string(top.front()) + "\". Item skipped.");
                            break;
                    }
                    if (object) {
                        imme = scope->add_imme(object);
                        top.pop(); // required to pop the operator rvalue
                        break;
                    }
                } else if (top.front().is_symbol())
                    symbol = top.front().front();
                else {
                    symbol = call_symbol;
                    goto inst_put;
                }
                top.pop();
            }
            args.pop();
            if (symbol == call_symbol)
                return inst_ends(scope, report);
            if (!imme.eof()) {
                Compound compound(imme, args.top());
                return scope->run_compound(compound);
            }
            return copy();
        }

        void exit(PUPS::Report &report) override {

        }

        [[nodiscard]] constexpr bool can_delete() const noexcept override {
            return true;
        }
    };


#undef INST_OP
#undef CALL_OP

    class TypeBase : public ObjectBase {
        signed char next_is_value = 0;
        std::stack<std::queue<Token>> names, args;

    protected:

        virtual ObjectPtr
        construct(Scope *scope, Report &report, std::queue<Token> &names, std::queue<Token> &args) = 0;

        virtual void type_ends(Scope *scope, Report &report) {}

        [[nodiscard]] virtual constexpr bool require_assign() const noexcept {
            return true;
        }

    public:
        TypeBase() {
            names.emplace();
            args.emplace();
        }

        void put(const Token &token, Report &report) final {
            if (require_assign()) {
                if (token.is_symbol('=')) {
                    next_is_value = 1;
                } else {
                    if (next_is_value) {
                        args.top().push(token);
                        next_is_value = -1;
                    } else
                        names.top().push(token);
                }
            } else {
                if (next_is_value) {
                    args.top().push(token);
                    next_is_value = -1;
                } else {
                    names.top().push(token);
                    next_is_value = 1;
                }
            }
        }

        ObjectPtr ends(Scope *scope, Report &report) final {
            type_ends(scope, report);
            auto &names_top = names.top();
            auto &args_top = args.top();
            names.emplace();
            args.emplace();
            auto result = construct(scope, report, names_top, args_top);
            names.pop();
            args.pop();
            if (!args_top.empty()) {
                report.report(Report_IncorrectArguments,
                              "Arguments are not enough for type initialization. (" + std::to_string(args.size()) +
                              " remaining)");
            }
            next_is_value = 0;
            return result;
        }

        void exit(PUPS::Report &report) override {

        }

        [[nodiscard]] Cnt type() const noexcept final {
            return TypeT_Cnt;
        }

        [[nodiscard]] virtual Cnt get_type() const noexcept {
            return cnt;
        }

        [[nodiscard]] constexpr bool is_type() const noexcept final {
            return true;
        }

        [[nodiscard]] constexpr bool can_delete() const noexcept override {
            return false;
        }
    };

    using InstancePtr = std::shared_ptr<InstanceBase>;
    using TypePtr = std::shared_ptr<TypeBase>;

    class TP_AnyT final : public TypeBase {
    public:
        TP_AnyT() = default;

        ObjectPtr construct(Scope *scope, Report &report, std::queue<Token> &names,
                            std::queue<Token> &args) override {
            report.report(Report_AnyTErr, "Cannot use anyt to construct anything.");
            return null_obj;
        }

        [[nodiscard]] Cnt get_type() const noexcept override {
            return AnyT_cnt;
        }
    };

    class TP_TypeT final : public TypeBase {
    public:
        TP_TypeT() = default;

        ObjectPtr construct(Scope *scope, Report &report, std::queue<Token> &names,
                            std::queue<Token> &args) override {
            report.report(Report_AnyTErr, "Cannot use typet to construct anything(even types).");
            return null_obj;
        }

        [[nodiscard]] Cnt get_type() const noexcept override {
            return TypeT_Cnt;
        }
    };

    class TP_NullT final : public TypeBase {
    public:
        TP_NullT() = default;

        ObjectPtr construct(Scope *scope, Report &report, std::queue<Token> &names,
                            std::queue<Token> &args) override {
            report.report(Report_AnyTErr, "Cannot use nullt to construct anything(even null itself).");
            return null_obj;
        }

        [[nodiscard]] Cnt get_type() const noexcept override {
            return NullT_Cnt;
        }
    };

    class StdTypeBase : public TypeBase {
    protected:
        static void chk_cst(Scope *scope, Report &report) {
            if (!scope->flags.make)
                report.report(Report_UnConstInit, std::string("Std type initialization with \"") + MAKE +
                                                  "\" qualifier is suggested. Statement continued in spite of warnings.");
        }

        void type_ends(PUPS::Scope *scope, PUPS::Report &report) final {
            chk_cst(scope, report);
        }
    };

#define OVERRIDE_OP(op_name, op, typeof_this) ObjectPtr op_name(const Token &nxt, Scope *scope, Report &report) override { \
    auto &object = scope->find(nxt);\
    this->chk_type_same(object, report);\
    return ObjectPtr{new typeof_this{this->type(), static_cast<Arith>(this->value op std::static_pointer_cast<typeof_this>(object)->value)}};\
}

#define BASIC_OP(typeof_this) \
OVERRIDE_OP(op_sub, -, typeof_this)\
OVERRIDE_OP(op_mul, *, typeof_this)\
OVERRIDE_OP(op_div, /, typeof_this)\
OVERRIDE_OP(op_lt, <, typeof_this)\
OVERRIDE_OP(op_gt, >, typeof_this)\
OVERRIDE_OP(op_eq, ==, typeof_this)

    template<typename Arith>
    class INST_Arith_Base : public InstanceBase {
    protected:
        Arith value;
        using Cnt = InstanceBase::Cnt;
    public:
        INST_Arith_Base(Cnt type, Arith value) : InstanceBase(type), value(value) {}

        [[nodiscard]] std::string to_string() const noexcept override {
            return std::to_string(value);
        }

        [[nodiscard]] constexpr bool to_condition() const noexcept override {
            return value != 0;
        }

        [[nodiscard]] ObjectPtr copy() const noexcept override {
            return ObjectPtr{new INST_Arith_Base{*this}};
        }
    };

    template<typename Arith>
    class INST_Arith_Int final : public INST_Arith_Base<Arith> {
        using Cnt = InstanceBase::Cnt;

        template<typename Arith_ = Arith>
        typename std::enable_if<std::is_same<Arith_, long long>::value, EvalResult::ResultType>::type
        get_type() const noexcept {
            return EvalResult::type_int;
        }

        template<typename Arith_ = Arith>
        typename std::enable_if<std::is_same<Arith_, unsigned char>::value, EvalResult::ResultType>::type
        get_type() const noexcept {
            return EvalResult::type_byte;
        }

        template<typename Arith_ = Arith>
        typename std::enable_if<std::is_same<Arith_, signed char>::value, EvalResult::ResultType>::type
        get_type() const noexcept {
            return EvalResult::type_schar;
        }

    public:
        INST_Arith_Int(Cnt type, Arith value) : INST_Arith_Base<Arith>(type, value) {}

        ObjectPtr op_add(const Token &nxt, Scope *scope, Report &report)
        override {
            auto &object = scope->find(nxt);
            this->
                    chk_type_same(object, report
            );
            return ObjectPtr{
                    new INST_Arith_Int{
                            this->
                                    type(),
                            static_cast<Arith>(this->value +
                                               std::static_pointer_cast<INST_Arith_Int>(object)
                                                       ->value)}};
        }

        BASIC_OP(INST_Arith_Int)

        OVERRIDE_OP(op_and, &, INST_Arith_Int)

        OVERRIDE_OP(op_or, |, INST_Arith_Int)

        OVERRIDE_OP(op_xor, ^, INST_Arith_Int)

        [[nodiscard]] EvalResult get_ev() const final {
            switch (get_type()) {
                case EvalResult::type_int:
                    return {EvalResult::type_int, {.r_int = static_cast<int>(this->value)}};
                case EvalResult::type_byte:
                    return {EvalResult::type_byte, {.r_byte = static_cast<unsigned char>(this->value)}};
                case EvalResult::type_schar:
                    return {EvalResult::type_schar, {.r_schar = static_cast<signed char>(this->value)}};
                default:
                    break;
            }
            return ObjectBase::get_ev();
        }

        [[nodiscard]]ObjectPtr copy() const noexcept override {
            return ObjectPtr{new INST_Arith_Int{*this}};
        }
    };

    template<typename Arith>
    class INST_Arith_Float final : public INST_Arith_Base<Arith> {
        using Cnt = InstanceBase::Cnt;
    public:
        INST_Arith_Float(Cnt type, Arith value) : INST_Arith_Base<Arith>(type, value) {}

        BASIC_OP(INST_Arith_Float)

        [[nodiscard]] EvalResult get_ev() const final {
            return {EvalResult::type_float, {.r_float = this->value}};
        }

        [[nodiscard]]ObjectPtr copy() const noexcept override {
            return ObjectPtr{new INST_Arith_Float{*this}};
        }
    };

#undef OVERRIDE_OP
#undef OVERRIDE_OP_BASE

    template<typename Arith>
    class TP_Arith final : public StdTypeBase {
    protected:
        template<typename T = Arith>
        typename std::enable_if<std::is_integral<T>::value, ObjectPtr>::type
        create(T value) {
            return ObjectPtr{new INST_Arith_Int<T>(cnt, value)};
        }

        template<typename T = Arith>
        typename std::enable_if<std::is_floating_point<T>::value, ObjectPtr>::type
        create(T value) {
            return ObjectPtr{new INST_Arith_Float<T>(cnt, value)};
        }

        ObjectPtr
        construct(Scope *scope, Report &report, std::queue<Token> &names, std::queue<Token> &args) override {
            ObjectPtr new_obj;
            std::string args_s = join_tokens(args);
            try {
                new_obj = create(eval<Arith>(args_s));
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

    public:
        TP_Arith() = default;
    };

    using INST_Int = INST_Arith_Int<long long>;
    using TP_Int = TP_Arith<long long>;

    using INST_Float = INST_Arith_Float<double>;
    using TP_Float = TP_Arith<double>;

    using INST_SChar = INST_Arith_Int<signed char>;
    using TP_SChar = TP_Arith<signed char>;

    using INST_Byte = INST_Arith_Int<unsigned char>;
    using TP_Byte = TP_Arith<unsigned char>;

    class INST_Str final : public InstanceBase {
        std::string value;
    protected:
        ObjectPtr op_add(const Token &nxt, Scope *scope, Report &report) override {
            auto &object = scope->find(nxt);
            chk_type_same(object, report);
            return ObjectPtr{new INST_Str(_type, value + std::static_pointer_cast<INST_Str>(object)->value)};
        }

        [[nodiscard]] ObjectPtr copy() const noexcept override {
            return ObjectPtr{new INST_Str{*this}};
        }

    public:
        INST_Str(Cnt type, std::string value) : InstanceBase(type), value(std::move(value)) {}

        [[nodiscard]] std::string to_string() const noexcept override {
            return "\"" + value + "\"";
        }

        [[nodiscard]] std::string to_repr() const noexcept override {
            return value;
        }

        [[nodiscard]] EvalResult get_ev() const final {
            return {EvalResult::type_str, {.r_str = &this->value}};
        }
    };

    class TP_Str : public StdTypeBase {
    protected:
        ObjectPtr
        construct(Scope *scope, Report &report, std::queue<Token> &names, std::queue<Token> &args) override {
            ObjectPtr new_obj;
            if (args.size() != 1 || !args.front().is_long()) {
                report.report(Report_IncorrectArguments,
                              "Str initialization requires one single long token. Statement skipped.");
                return null_obj;
            }
            const std::string &arg = args.front().long_cut();
            args.pop();
            new_obj = ObjectPtr{
                    new INST_Str(cnt, arg)};
            while (!names.empty()) {
                scope->set_object(names.front(), new_obj);
                names.pop();
            }
            return new_obj;
        }

    public:
        TP_Str() = default;
    };

    namespace TypeCodes {
        ObjectBase::Cnt Int, Float, Str, Byte, SChar;
    }
}

#endif //POURUPSCRIPTTEST_STDTYPE_HPP
