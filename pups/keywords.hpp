//
// Created by Dogs-Cute on 4/22/2023.
//

#ifndef POURUPSCRIPTTEST_KEYWORDS_HPP
#define POURUPSCRIPTTEST_KEYWORDS_HPP

#include "stdtype.hpp"

namespace PUPS {
    class KeywordBase : public ObjectBase {
    public:
        [[nodiscard]] bool can_delete() const noexcept final {
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
            if (value.null()) {
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

    class KW_Sco final : public KeywordBase {
        struct ScopeInit {
            Token name, block;
        };
        std::stack<ScopeInit> inits;
        signed char status = -1;
    public:
        KW_Sco() {
            inits.emplace();
        }

        void put(const Token &token, Report &report) override {
            switch (status) {
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
            status++;
        }

        ObjectPtr ends(Scope *scope, Report &report) override {
            auto &top = inits.top();
            inits.emplace();
            make_scope(top.name, top.block, scope, report);
            inits.pop();
            status = -1;
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
                fpath path = report.find_file(pair.first.str());
                if (path.empty())
                    report.report(Report_FileNotFound, "Include file \"" + pair.first.str() + "\" is not found.");
                else
                    make_scope(pair.second, path, scope, report);
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
                scope->get_report().paths.emplace_back(paths.front().str_dependent());
                paths.pop();
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
                if (stmt.empty())
                    report.report(Report_IncorrectArguments,
                                  std::string("No statement after \"") + get_name() + "\" qualifier.");
                else {
                    Compound compound(stmt);
                    scope->run_compound(compound);
                }
            }
            return null_obj;
        }
    };

    class KW_Cst final : public KeywordQualifier {
        void set_v(Scope *scope) override {
            scope->flags.cst = true;
        }

        const char *get_name() override {
            return CST;
        }

    public:
        KW_Cst() = default;
    };

    class KW_Loc : public KeywordQualifier {
        void set_v(Scope *scope) override {
            scope->flags.loc = true;
        }

        const char *get_name() override {
            return LOC;
        }

    public:
        KW_Loc() = default;
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

    class Keywords {
        std::unordered_map<Token, ObjectPtr> keywords, types;
    public:
        Keywords() : keywords{
                {Token{NULL_OBJ}, null_obj},
                {Token{DECL},     ObjectPtr{new KW_Decl}},
                {Token{PRINT},    ObjectPtr{new KW_Print}},
                {Token{SET},      ObjectPtr{new KW_Set}},
                {Token{CST},      ObjectPtr{new KW_Cst}},
                {Token{LOC},      ObjectPtr{new KW_Loc}},
                {Token{SCO},      ObjectPtr{new KW_Sco}},
                {Token{NOTHING},  ObjectPtr{new KW_Nothing}},
                {Token{INCL},     ObjectPtr{new KW_Incl}},
                {Token{COMM},     ObjectPtr{new KW_Comm}},
                {Token{AGAIN},    ObjectPtr{new KW_Again}},
                {Token{ADD_PATH}, ObjectPtr{new KW_Add_Path}},
                {Token{NEW},      ObjectPtr{new KW_New}},
        },
                     types{
                             {Token{INT},   ObjectPtr{new TP_Int}},
                             {Token{FLOAT}, ObjectPtr{new TP_Float}},
                             {Token{STR},   ObjectPtr{new TP_Str}}
                     } {}

        ~Keywords() = default;

        virtual void merge_to(Scope &scope) {
            scope.objects.merge(keywords);
            scope.objects.merge(types);
        }
    };
}

#endif //POURUPSCRIPTTEST_KEYWORDS_HPP
