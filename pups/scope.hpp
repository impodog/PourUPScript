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

        Compound(const Token& beg, std::queue<Token> &args) {
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
        Report &_report;
        Scope *parent = nullptr;


    public:
        struct Flags {
        private:
            bool colon = false;
            bool find_no_err = false;

            void reset() {
                colon = find_no_err = cst = loc = false;
            }

            friend class Scope;

        public:
            bool cst = false, loc = false;

        } flags;

        explicit Scope(Scope *parent, Report &report) :
                _report(report), parent(parent) {
            nests.emplace();
        }

        ~Scope() override {
            _exit();
        }

        Token add_imme(const ObjectPtr &object) {
            Token token = next_tag();
            objects.insert({token, object});
            imme_names.push(token);
            return token;
        }

        ObjectPtr &find(const Token &name) {
            bool _tmp;
            return find(name, _tmp);
        }

        template<bool cancel_warn>
        typename std::enable_if<cancel_warn, ObjectPtr &>::type find(const Token &name) {
            bool _tmp;
            flags.find_no_err = true;
            auto &result = find(name, _tmp);
            flags.find_no_err = false;
            return result;
        }

        template<bool cancel_warn>
        typename std::enable_if<!cancel_warn, ObjectPtr &>::type find(const Token &name) {
            return find(name);
        }

        ObjectPtr &find(const Token &name, bool &is_local) {
            Scope *scope = this;
            is_local = true;
            while (true) {
                try {
                    return scope->objects.at(name);
                } catch (const std::out_of_range &) {
                    if (scope->parent == nullptr) {
                        if (!flags.find_no_err)
                            _report.report(Report_UnknownName, "Cannot find name \"" + std::string(name) + "\".");
                        return null_obj;
                    }
                    is_local = false;
                    scope = scope->parent;
                }
            }
        }

        template<bool cancel_warn>
        typename std::enable_if<cancel_warn, ObjectPtr &>::type find(const Token &name, bool &is_local) {
            flags.find_no_err = true;
            auto &result = find(name, is_local);
            flags.find_no_err = false;
            return result;
        }

        template<bool cancel_warn>
        typename std::enable_if<!cancel_warn, ObjectPtr &>::type find(const Token &name, bool &is_local) {
            return find(name, is_local);
        }

        ObjectPtr run_compound(Compound &compound) {
            if (compound.empty()) return null_obj;
            size_t status = 0;
            ObjectPtr acceptor;
            compound.filter();
            bool colon_mode = false;
            for (auto &token: compound.tokens) {
                if (status++) {
                    if (colon_mode)
                        return find(token);// fixme it doesnt work and returns null_obj
                    else
                        acceptor->put(token, _report);
                } else {
                    if (token.is_symbol() && token.colon())
                        colon_mode = true;
                    else
                        acceptor = find(token);
                }
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
            bool is_loc;
            auto &f = find<true>(token, is_loc);
            if (f == null_obj || !is_loc && flags.loc) {
                if (!flags.loc)
                    _report.report(Report_Undeclared,
                                   "Setting name \"" + std::string(token) +
                                   "\", which is not declared. Unexpected things may happen.");
                try_exit_erase_object(token);
                objects.insert({token, object});
            } else f = object;
        }

        void declare_object(const Token &token) {
            if (objects.find(token) == objects.end())
                objects.insert({token, nullptr});
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
            while (!nests.empty()) {
                add_imme(run_compound(nests.top()));
                nests.top().exit(_report);
                nests.pop();
            }
            nests.emplace();
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

        [[nodiscard]] bool is_scope() const noexcept final {
            return true;
        }

        Report &get_report() noexcept {
            return _report;
        }
    };

    ObjectPtr make_scope(const Token &name, const fpath &path, Scope *parent, Report &report);

    ObjectPtr make_scope(const Token &name, const Token &block, Scope *parent, Report &report);
}

#endif //POURUPSCRIPT_SCOPE_HPP
