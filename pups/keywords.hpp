//
// Created by Dogs-Cute on 4/22/2023.
//

#ifndef POURUPSCRIPTTEST_KEYWORDS_HPP
#define POURUPSCRIPTTEST_KEYWORDS_HPP

#include "stdtype.hpp"

#define DECL "decl"
#define REF "ref"
#define PRINT "print"
#define SET "set"

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
                              DECL " should only get alphabetic names. Item \"" + std::string(token) + "\" skipped.");
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
                *Report::output << scope->find(args.front())->to_string();
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
                                  SET "should only get \"=\" as assign parameter. Item \"" + std::string(token) +
                                  "\" skipped.");
            } else {
                if (next_is_value) {
                    if (next_is_value < 0)
                        report.report(Report_WrongToken,
                                      SET "should only make one assignment once. Item \"" + std::string(token) +
                                      "skipped.");
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

    class KW_Cst final : public KeywordBase {
        std::queue<Token> stmt;
    public:
        KW_Cst() = default;

        void put(const Token &token, Report &report) override {
            stmt.push(token);
        }

        ObjectPtr ends(Scope *scope, Report &report) override {
            scope->flags.cst = true;
            if (stmt.empty())
                report.report(Report_IncorrectArguments, "No statement after \"cst\" qualifier.");
            else {
                Compound compound(stmt);
                scope->run_compound(compound);
            }
            return null_obj;
        }
    };

    class Keywords {
        std::unordered_map<Token, ObjectPtr> keywords, types;
    public:
        Keywords() : keywords{
                {Token{NULL_OBJ, false}, null_obj},
                {Token{DECL, false},     ObjectPtr{new KW_Decl}},
                {Token{PRINT, false},    ObjectPtr{new KW_Print}},
                {Token{SET, false},      ObjectPtr{new KW_Set}},
                {Token{CST, false},      ObjectPtr{new KW_Cst}}
        },
                     types{
                             {Token{INT, false},   ObjectPtr{new TP_Int}},
                             {Token{FLOAT, false}, ObjectPtr{new TP_Float}}
                     } {}

        ~Keywords() = default;

        virtual void merge_to(Scope &scope) {
            scope.objects.merge(keywords);
            scope.objects.merge(types);
        }
    };
}

#endif //POURUPSCRIPTTEST_KEYWORDS_HPP
