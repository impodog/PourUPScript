//
// Created by Dogs-Cute on 4/22/2023.
//

#ifndef POURUPSCRIPTTEST_KEYWORDS_HPP
#define POURUPSCRIPTTEST_KEYWORDS_HPP

#include "function.hpp"

namespace PUPS {
    class KeywordBase : public ObjectBase {
    public:
        [[nodiscard]] constexpr bool can_delete() const noexcept final {
            return false;
        }

        void exit(PUPS::Report &report) final {

        }
    };

    class KW_Decl final : public KeywordBase {
        std::queue<Token> declares;
    public:
        KW_Decl() = default;

        void put(const Token &token, Report &report) override {
            if (token.is_symbol())
                report.report(Report_WrongToken,
                              std::string("\"") + DECL + "\" should only get alphabetic names. Item \"" +
                              std::string(token) +
                              "\" skipped.");
            else
                declares.push(token);
        }

        ObjectPtr ends(PUPS::Scope *scope, PUPS::Report &report) override {
            while (!declares.empty()) {
                scope->declare_object(declares.front());
                declares.pop();
            }
            return null_obj;
        }
    };

    class KW_Print final : public KeywordBase {
        std::queue<Token> args;
    public:
        void put(const Token &token, Report &report) override {
            args.push(token);
        }

        ObjectPtr ends(PUPS::Scope *scope, PUPS::Report &report) override {
            while (!args.empty()) {
                *Report::output << scope->find(args.front())->to_repr();
                args.pop();
            }
            *Report::output << std::endl;
            return null_obj;
        }
    };

    class KW_Set final : public KeywordBase {
        std::queue<Token> names;
        Token value;
        signed char next_is_value = 0;
    public:
        KW_Set() = default;

        void put(const Token &token, Report &report) override {
            if (token.is_symbol()) {
                if (token.front() == '=') {
                    next_is_value = 1;
                } else
                    report.report(Report_WrongToken,
                                  std::string("\"") + SET + R"(" should only get "=" as assign parameter. Item ")" +
                                  std::string(token) +
                                  "\" skipped.");
            } else {
                if (next_is_value) {
                    if (next_is_value < 0)
                        report.report(Report_WrongToken,
                                      std::string("\"") + SET + "\" should only make one assignment once. Item \"" +
                                      std::string(token) +
                                      "\" skipped.");
                    else {
                        value = token;
                        next_is_value = -1;
                    }
                } else
                    names.push(token);
            }
        }

        ObjectPtr ends(PUPS::Scope *scope, PUPS::Report &report) override {
            if (!next_is_value) {
                report.report(Report_IncorrectArguments, "No assignment symbol is present. Statement skipped.");
                return null_obj;
            } else if (value.null()) {
                report.report(Report_IncorrectArguments, "No assignment value is given. Statement skipped.");
                return null_obj;
            }
            auto assign_val = scope->find(value);
            while (!names.empty()) {
                scope->set_object(names.front(), assign_val);
                names.pop();
            }
            next_is_value = 0;
            return null_obj;
        }
    };

    class KW_Scope final : public KeywordBase {
        struct ScopeInit {
            Token name, block;
        };
        std::stack<ScopeInit> inits;
        std::stack<signed char> status;
    public:
        KW_Scope() {
            inits.emplace();
            status.emplace(-1);
        }

        void put(const Token &token, Report &report) override {
            switch (status.top()) {
                case -1:
                    inits.top().name = token;
                    break;
                case 0:
                    inits.top().block = token;
                    break;
                default:
                    report.report(Report_IncorrectArguments,
                                  "An extra argument \"" + std::string(token) +
                                  "\" for keyword \"" + SCO + "\". Token skipped.");
                    break;
            }
            status.top()++;
        }

        ObjectPtr ends(Scope *scope, Report &report) override {
            auto &top = inits.top();
            if (top.block.empty()) swap(top.name, top.block);
            inits.emplace();
            status.emplace(-1);
            create_scope(top.name, top.block, scope, report);
            inits.pop();
            status.pop();
            status.top() = -1;
            return null_obj;
        }
    };

    class KW_Incl final : public KeywordBase {
        std::stack<std::list<std::pair<Token, Token>>> names;
    public:
        static constexpr const char *const incl_as = "->";

        KW_Incl() {
            names.emplace();
        }

        void put(const Token &token, Report &report) override {
            if (token.is_symbol())
                report.report(Report_IncorrectArguments,
                              std::string("\"") + INCL + "\" should get long or normal tokens as parameter. Item \"" +
                              std::string(token) + "\" skipped.");
            else {
                std::string argument = token.str_dependent_no_space();
                auto assign = argument.find(incl_as);
                if (assign == std::string::npos)
                    names.top().emplace_back(token, token);
                else
                    names.top().emplace_back(Token{argument.substr(0, assign)},
                                             Token{argument.substr(assign + strlen(incl_as))});
            }
        }

        ObjectPtr ends(Scope *scope, Report &report) override {
            for (auto &pair: names.top()) {
                names.emplace();
                include_path(scope, report, pair);
                names.pop();
            }
            return null_obj;
        }
    };

    class KW_Comm : public KeywordBase {
        size_t token_count = 0;
    public:
        KW_Comm() = default;

        void put(const Token &token, Report &report) override {
            if (token_count || token.is_long()) {
                if (++token_count > 1)
                    report.report(Report_SuspiciousLine,
                                  "Possible missing colon after comments line. Causes the desired statement to be skipped.");
            }
        }

        ObjectPtr ends(Scope *scope, Report &report) override {
            token_count = 0;
            return null_obj;
        }
    };

    class KW_Add_Path final : public KeywordBase {
        std::queue<Token> paths;
    public:
        KW_Add_Path() = default;

        void put(const Token &token, Report &report) override {
            paths.push(token);
        }

        ObjectPtr ends(Scope *scope, Report &report) override {
            while (!paths.empty()) {
                scope->get_report().paths->emplace_back(paths.front().str_dependent());
                paths.pop();
            }
            return null_obj;
        }
    };

    class KW_Remove final : public KeywordBase {
        std::queue<Token> names;
    public:
        KW_Remove() = default;

        void put(const Token &token, Report &report) override {
            names.push(token);
        }

        ObjectPtr ends(Scope *scope, Report &report) override {
            while (!names.empty()) {
                scope->erase_object(names.front());
                names.pop();
            }
            return null_obj;
        }
    };

    class KW_Ret final : public KeywordBase {
        Token name;
    public:
        KW_Ret() = default;

        void put(const Token &token, Report &report) override {
            if (name.null())
                name = token;
            else
                report.report(Report_IncorrectArguments,
                              "Extra argument \"" + std::string(token) + "\" in return statement");
        }

        ObjectPtr ends(Scope *scope, Report &report) override {
            if (name.null()) {
                report.report(Report_IncorrectArguments, "No return value is given. Statement skipped.");
                return null_obj;
            }
            scope->flags.returned = true;
            auto result = scope->find(name);
            name = Token{};
            scope->set_object<true>(Token{RETURN_V}, result);
            return null_obj;
        }
    };

    class KW_If final : public KeywordBase {
        struct Condition {
            unsigned char status = 0;
            Token cond, ifTrue, ifFalse = Token{"{}", Token::type_long};

            void reset() {
                status = 0;
                ifFalse = Token{"{}", Token::type_long};
            }

            void add_semicolon() {
                if (!ifFalse.has_semicolon()) ifFalse.append(';');
            }
        };

        std::stack<Condition> conditions;
    public:
        static constexpr const cstr ELSE = "else";

        KW_If() {
            conditions.emplace();
        }

        void put(const Token &token, Report &report) override {
            auto &top = conditions.top();
            switch (top.status) {
                case 0:
                    top.cond = token;
                    break;
                case 1:
                    top.ifTrue = token;
                    break;
                case 2:
                    if (token != ELSE) top.status = 4 - 1;
                    break;
                case 3:
                    top.ifFalse = token;
                    if (token == IF) top.status = 5 - 1;
                    break;
                case 4:
                    report.report(Report_IncorrectArguments,
                                  "Too many arguments for an if statement. \"" + token.str() + "\" skipped.");
                    return;
                case 5:
                    top.ifFalse.append(' ').append(token);
                    return;
            }
            top.status++;
        }

        ObjectPtr ends(Scope *scope, Report &report) override {
            Token *block;
            {
                auto &top = conditions.top();
                if (top.status == 5) {
                    top.add_semicolon();
                    top.ifFalse.convert_to_long();
                }
                auto &result = scope->find<true>(top.cond);
                if (result == nullptr || result == null_obj) {
                    report.report(Report_IncorrectArguments,
                                  "Condition \"" + std::string(top.cond) +
                                  "\" is not defined. Condition is always false.");
                    block = &top.ifFalse;
                } else if (result->to_condition()) block = &top.ifTrue;
                else block = &top.ifFalse;
            }
            Token name = next_tag();
            conditions.emplace();
            create_scope(name, *block, scope, report);
            scope->try_exit_erase_object(name);
            conditions.pop();
            if (conditions.size() == 1)
                conditions.top().reset();
            return null_obj;
        }
    };

    class KW_While final : public KeywordBase {
        struct Condition {
            unsigned char status = 0;
            Token cond, body = Token{"{}", Token::type_long};

            void reset() {
                status = 0;
                body = Token{"{}", Token::type_long};
            }
        };

        std::stack<Condition> conditions;

        static ObjectPtr get_cond(const Condition &cond, Scope *scope, Report &report) {
            auto &object = scope->find(cond.cond);
            if (object->is_instance())
                return std::static_pointer_cast<InstanceBase>(object)->call_without_args(scope, report);
            report.report(Report_TypeErr,
                          std::string(WHILE) + " statement must receive a callable instance as condition.");
            return null_obj;
        }

        void work(ObjectPtr &result, Condition &cond, Scope *scope, Report &report) {
            Token name = next_tag();
            conditions.emplace();
            create_scope(name, cond.body, scope, report);
            scope->try_exit_erase_object(name);
            result = get_cond(cond, scope, report);
            conditions.pop();
        }

    public:
        KW_While() {
            conditions.emplace();
        }

        void put(const Token &token, Report &report) override {
            auto &top = conditions.top();
            switch (top.status) {
                case 0:
                    top.cond = token;
                    break;
                case 1:
                    top.body = token;
                    break;
                case 2:
                    return;
            }
            top.status++;
        }

        ObjectPtr ends(Scope *scope, Report &report) override {
            auto &top = conditions.top();
            ObjectPtr result = get_cond(top, scope, report);
            for (size_t i = 0; i < scope->flags.pre_do; i++)
                work(result, top, scope, report);
            while (result != nullptr && result->to_condition())
                work(result, top, scope, report);
            if (conditions.size() == 1)
                conditions.top().reset();
            return null_obj;
        }
    };

    class KW_Number final : public KeywordBase {
        struct Number {
            Token value;
            bool floating, ok;
        };
        std::queue<Number> numbers;

        static Number check_number(const Token &token) {
            bool dot = false;
            for (auto c: token.str()) {
                if (c == '.') {
                    if (dot) return {token, false, false};
                    dot = true;
                } else if (!is_digit(c)) return {token, false, false};
            }
            return {token, dot, true};
        }

    public:
        void put(const Token &token, Report &report) override {
            auto num = check_number(token);
            if (num.ok)
                numbers.push(num);
            else
                report.report(Report_WrongToken,
                              "Cannot make number using token \"" + std::string(token) + "\". Token skipped.");
        }

        ObjectPtr ends(Scope *scope, Report &report) override {
            if (!scope->flags.make)
                report.report(Report_SuggestMakeInit, std::string("Using \"") + MAKE +
                                                      "\" along with \"" + NUMBER + "\" is suggested.");
            while (!numbers.empty()) {
                ObjectPtr object;
                if (numbers.front().floating)
                    object = std::make_shared<INST_Float>(TypeCodes::Float, eval<double>(numbers.front().value.str()));
                else
                    object = std::make_shared<INST_Int>(TypeCodes::Int, eval<long long>(numbers.front().value.str()));
                scope->set_object<true>(numbers.front().value, object);
                numbers.pop();
            }
            return null_obj;
        }
    };

    class KW_Using final : public KeywordBase {
        std::queue<Token> using_ns;
    public:
        void put(const Token &token, Report &report) override {
            using_ns.push(token);
        }

        ObjectPtr ends(Scope *scope, Report &report) override {
            while (!using_ns.empty()) {
                auto object = scope->find(using_ns.front());
                if (!object->is_scope())
                    report.report(Report_TypeErr, "Cannot use \"" + std::string(using_ns.front()) +
                                                  "\" as a using scope. Token skipped.");
                else
                    scope->copy_objects_from(std::static_pointer_cast<Scope>(object).get());
                using_ns.pop();
            }
            return null_obj;
        }
    };


    class KeywordQualifier : public KeywordBase {
        std::queue<Token> stmt;

    protected:
        virtual void set_v(Scope *scope) = 0;

        virtual const char *get_name() = 0;

        virtual bool check(Scope *scope, Report &report) {
            return true;
        }

    public:
        explicit KeywordQualifier() = default;

        void put(const Token &token, Report &report) final {
            stmt.push(token);
        }

        ObjectPtr ends(Scope *scope, Report &report) final {
            if (check(scope, report)) {
                set_v(scope);
                if (stmt.empty()) {
                    report.report(Report_IncorrectArguments,
                                  std::string("No statement after \"") + get_name() + "\" qualifier.");
                    return null_obj;
                } else {
                    Compound compound(stmt);
                    return scope->run_compound(compound);
                }
            }
            return null_obj;
        }
    };

    class KW_Make final : public KeywordQualifier {
        void set_v(Scope *scope) override {
            scope->flags.make = true;
        }

        const char *get_name() override {
            return MAKE;
        }

    public:
        KW_Make() = default;
    };

    class KW_Local : public KeywordQualifier {
        void set_v(Scope *scope) override {
            scope->flags.loc = true;
        }

        const char *get_name() override {
            return LOC;
        }

    public:
        KW_Local() = default;
    };

    class KW_Nothing final : public KeywordQualifier {
        void set_v(Scope *scope) override {}

        const char *get_name() override {
            return NOTHING;
        }

    public:
        KW_Nothing() = default;
    };

    class KW_Again final : public KeywordQualifier {
        void set_v(Scope *scope) override {
            scope->flags.again = true;
        }

        const char *get_name() override {
            return AGAIN;
        }

    public:
        KW_Again() = default;
    };

    class KW_New final : public KeywordQualifier {
        void set_v(Scope *scope) override {
            scope->flags.new_decl = true;
        }

        const char *get_name() override {
            return NEW;
        }

    public:
        KW_New() = default;
    };

    class KW_Do final : public KeywordQualifier {
        void set_v(Scope *scope) override {
            scope->flags.pre_do++;
        }

        const char *get_name() override {
            return DO;
        }

    public:
        KW_Do() = default;
    };


    class Keywords {
        std::unordered_map<Token, ObjectPtr> keywords, types, constants;
    public:
        Keywords() : keywords{
                {Token{NULL_OBJ}, null_obj},
                {Token{DECL},     ObjectPtr{new KW_Decl}},
                {Token{PRINT},    ObjectPtr{new KW_Print}},
                {Token{SET},      ObjectPtr{new KW_Set}},
                {Token{MAKE},     ObjectPtr{new KW_Make}},
                {Token{LOC},      ObjectPtr{new KW_Local}},
                {Token{SCO},      ObjectPtr{new KW_Scope}},
                {Token{NOTHING},  ObjectPtr{new KW_Nothing}},
                {Token{INCL},     ObjectPtr{new KW_Incl}},
                {Token{COMM},     ObjectPtr{new KW_Comm}},
                {Token{AGAIN},    ObjectPtr{new KW_Again}},
                {Token{ADD_PATH}, ObjectPtr{new KW_Add_Path}},
                {Token{NEW},      ObjectPtr{new KW_New}},
                {Token{REMOVE},   ObjectPtr{new KW_Remove}},
                {Token{RETURN},   ObjectPtr{new KW_Ret}},
                {Token{IF},       ObjectPtr{new KW_If}},
                {Token{WHILE},    ObjectPtr{new KW_While}},
                {Token{DO},       ObjectPtr{new KW_Do}},
                {Token{NUMBER},   ObjectPtr{new KW_Number}},
                {Token{USING},    ObjectPtr(new KW_Using)}
        },
                     types{
                             {Token{INT},      ObjectPtr{new TP_Int}},
                             {Token{FLOAT},    ObjectPtr{new TP_Float}},
                             {Token{STR},      ObjectPtr{new TP_Str}},
                             {Token{BYTE},     ObjectPtr{new TP_Byte}},
                             {Token{SCHAR},    ObjectPtr{new TP_SChar}},
                             {Token{FUNCTION}, ObjectPtr{new TP_Function}},
                             {Token{ANYT},     ObjectPtr{new TP_AnyT}},
                             {Token{TYPET},    ObjectPtr{new TP_TypeT}},
                             {Token{NULLT},    ObjectPtr{new TP_NullT}},
                             {Token{LAMBDA},   ObjectPtr{new TP_LambdaFunction}}
                     } {
            TypeCodes::Int = types.at(Token{INT})->cnt;
            TypeCodes::Float = types.at(Token{FLOAT})->cnt;
            TypeCodes::Str = types.at(Token{STR})->cnt;
            TypeCodes::Byte = types.at(Token{BYTE})->cnt;
            TypeCodes::SChar = types.at(Token{SCHAR})->cnt;

            if (!false_obj)
                false_obj = ObjectPtr{new INST_Byte(TypeCodes::Byte, 0)};
            if (!true_obj)
                true_obj = std::make_shared<INST_Byte>(TypeCodes::Byte, 1);
            constants.insert({Token{FALSE_OBJ}, false_obj});
            constants.insert({Token{TRUE_OBJ}, true_obj});
        }

        ~Keywords() = default;

        virtual void merge_to(Scope &scope) {
            scope.objects.merge(keywords);
            scope.objects.merge(types);
            scope.objects.merge(constants);
            scope.objects.merge(builtins);
        }
    };
}

#endif //POURUPSCRIPTTEST_KEYWORDS_HPP
