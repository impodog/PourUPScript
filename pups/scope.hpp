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
            tokens.push_back(beg);
            while (!args.empty()) {
                tokens.push_back(args.front());
                args.pop();
            }
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
            std::cout << "call" << std::endl;
            for (auto &pair: objects) {
                std::cout << pair.first << std::endl;
                if (pair.second == nullptr)
                    _report.report(Report_Uninitialized, "When exiting Scope, name \"" + std::string(pair.first) +
                                                         "\" is a declared, uninitialized object(nullptr). Item skipped.");
                else if (!pair.first.str().starts_with(TAG) && pair.second.use_count())
                    exit_object(pair.first);
                std::cout << pair.first << std::endl;
            }
            objects.clear();
        }

        friend class Keywords;

    protected:
        std::unordered_map<Token, ObjectPtr> objects;
        Report _report;
        Scope *parent = nullptr;


    public:
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
                    auto &object = scope->objects.at(name);
                    if (object) return object;
                    else
                        throw PUPS_FatalError(
                                "when finding, name \"" + std::string(name) +
                                "\" is a declared, uninitialized object(nullptr)",
                                _report);
                } catch (const std::out_of_range &) {
                    if (scope->parent == nullptr) {
                        throw PUPS_FatalError("Cannot find name \"" + std::string(name) + "\".", _report);
                    }
                    scope = scope->parent;
                }
            }
        }

        ObjectPtr run_compound(Compound &compound) {
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
            try {
                auto &f = find(token);
                f = object;
            } catch (const PUPS_FatalError &) {
                _report.report(Report_Undeclared,
                               "Setting name \"" + std::string(token) +
                               "\", which is not declared. Unexpected things may happen.");
                try_exit_erase_object(token);
                objects.insert({token, object});
            }
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
