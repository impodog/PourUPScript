//
// Created by Dogs-Cute on 4/22/2023.
//

#ifndef POURUPSCRIPT_SCOPE_HPP
#define POURUPSCRIPT_SCOPE_HPP

#include "object.hpp"

namespace PUPS {
    class Compound : public ObjectBase {
    protected:
        std::vector<Token> tokens;

        friend class Scope;

        friend std::ostream &operator<<(std::ostream &out, const Compound &compound);

    public:
        Compound() = default;

        Compound(Token beg, std::queue<Token> &args) {
            if (!beg.eof() && !beg.empty())
                tokens.push_back(beg);
            while (!args.empty()) {
                tokens.push_back(args.front());
                args.pop();
            }
        }

        explicit Compound(std::queue<Token> &args) {
            while (!args.empty()) {
                tokens.push_back(args.front());
                args.pop();
            }
        }

        [[nodiscard]] bool empty() const noexcept {
            return tokens.empty();
        }

        void put(const Token &token, Report &report) override {
            if (!token.empty())
                tokens.push_back(token);
        }

        ObjectPtr ends(PUPS::Scope *scope, PUPS::Report &report) override {
            if (tokens.empty())
                report.report(Report_EmptyCompound, "Ending statement with empty compound.");
            return null_obj;
        }

        void exit(Report &report) override {
            tokens.clear();
        }

        void filter() {
            while (tokens.back().empty())
                tokens.pop_back();
        }
    };

    std::ostream &operator<<(std::ostream &out, const Compound &compound) {
        for (const auto &token: compound.tokens)
            out << token << " ";
        return out;
    }

    class Scope : public ObjectBase {
        std::stack<Compound> nests;
        std::queue<Token> imme_names;

        void _exit() {
            for (auto &pair: objects) {
                if (pair.second == nullptr)
                    _report.report(Report_Uninitialized, "When exiting Scope, name \"" + std::string(pair.first) +
                                                         "\" is a declared, uninitialized object(nullptr). Item skipped.");
                else if (!pair.first.str().starts_with(TAG) && pair.second.use_count())
                    exit_object(pair.first);
            }
            objects.clear();
        }

        friend class Keywords;

    protected:
        std::unordered_map<Token, ObjectPtr> objects;
        Report _report;
        Scope *parent = nullptr;


    public:
        struct Flags {
            bool find_no_null_warn = false, found_null = false;
            bool cst = false;

            void reset() {
                find_no_null_warn = found_null = cst = false;
            }
        } flags;

        Token add_imme(const ObjectPtr &object) {
            Token token = next_tag();
            objects.insert({token, object});
            imme_names.push(token);
            return token;
        }

        ObjectPtr &find(const Token &name) {
            Scope *scope = this;
            while (true) {
                try {
                    return scope->objects.at(name);
                } catch (const std::out_of_range &) {
                    if (scope->parent == nullptr) {
                        _report.report(Report_UnknownName, "Cannot find name \"" + std::string(name) + "\".");
                        return null_obj;
                    }
                    scope = scope->parent;
                }
            }
        }

        ObjectPtr run_compound(Compound &compound) {
            if (compound.empty()) return null_obj;
            size_t status = 0;
            ObjectPtr acceptor;
            compound.filter();
            for (auto &token: compound.tokens) {
                if (status++)
                    acceptor->put(token, _report);
                else
                    acceptor = find(token);
            }
            return acceptor->ends(this, _report);
        }

        void exit_object(const Token &token) {
            objects.at(token)->exit(_report);
        }

        void erase_object(const Token &token) {
            objects.erase(token);
        }

        void try_exit_erase_object(const Token &token) {
            if (objects.find(token) != objects.end()) {
                if (objects.at(token) != nullptr)
                    exit_object(token);
                erase_object(token);
            }
        }

        void set_object(const Token &token, const ObjectPtr &object) {
            flags.find_no_null_warn = true;
            auto &f = find(token);
            if (!flags.found_null && f == null_obj) {
                _report.report(Report_Undeclared,
                               "Setting name \"" + std::string(token) +
                               "\", which is not declared. Unexpected things may happen.");
                try_exit_erase_object(token);
                objects.insert({token, object});
            } else f = object;
            flags.found_null = false;
        }

        void declare_object(const Token &token) {
            if (objects.find(token) == objects.end())
                objects.insert({token, nullptr});
        }

        explicit Scope(const TokenInput &tokenInput, Scope *parent) :
                _report(tokenInput), parent(parent) {
            nests.emplace();
        }

        ~Scope() override {
            _exit();
        }

        void put(const Token &token, Report &report) override {
            Token tmp;
            if (token.is_symbol())
                switch (token.front()) {
                    case '(':
                        nests.emplace();
                        break;
                    case ')':
                        tmp = add_imme(run_compound(nests.top()));
                        nests.pop();
                        nests.top().put(tmp, _report);
                        break;
                    case ';':
                        break;
                    default:
                        goto add_token;
                }
            else
                add_token:
                nests.top().put(token, _report);
        }

        ObjectPtr ends(PUPS::Scope *scope, PUPS::Report &report) override {
            if (nests.size() != 1)
                report.report(Report_AbnormalStack, "Statement ending with stacks unclosed.");
            add_imme(run_compound(nests.top()));
            nests.top().exit(_report);
            while (!imme_names.empty()) {
                erase_object(imme_names.front());
                imme_names.pop();
            }
            flags.reset();
            return null_obj;
        }

        void exit(Report &report) override {
            _exit();
        }

        Report &reports() noexcept {
            return _report;
        }
    };
}

#endif //POURUPSCRIPT_SCOPE_HPP
