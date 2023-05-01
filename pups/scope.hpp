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

        Compound(const Token &beg, std::queue<Token> &args) {
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

        ObjectPtr &find_no_stage(const Token &name, bool &is_local) {
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

        void exit_no_stage(const Token &token) {
            objects.at(token)->exit(_report);
        }

        void erase_no_stage(const Token &token) {
            objects.erase(token);
        }

        void declare_no_stage(const Token &token) {
            flags.get(parent);
            bool is_local;
            flags.find_no_err = true;
            auto result = find_no_stage(token, is_local);
            flags.find_no_err = false;
            if (is_non(result)) {
                if (flags.again) {
                    _report.report(Report_AgainWarning,
                                   "Declaration of \"" + std::string(token) + "\" qualified with " +
                                   wrap_quotes(AGAIN) +
                                   " should be initialized. Statement skipped.");
                    return;
                }
            } else if (flags.new_decl && is_local) {
                _report.report(Report_NewWarning,
                               "Declaration of \"" + std::string(token) + "\" qualified with " + wrap_quotes(NEW) +
                               " should NOT be initialized. Statement skipped.");
                return;
            }
            if (!flags.again) // This is used to prevent again-qualified declaration from creating a new one
                objects.insert({token, nullptr});
        }

        // This will pop the queue until the last element
        Scope *cut_scope_from(decltype(Token().split_stages()) &stages, bool &is_local) {
            Scope *ptr = this;
            while (stages.size() != 1) {
                auto p = ptr->find_no_stage(Token{stages.front()}, is_local).get();
                if (p == nullptr || !p->is_scope())
                    _report.report(Report_TypeErr,
                                   "Colon shortcuts must prefix with initialized scopes. Not \"" + stages.front() +
                                   "\".");
                else ptr = dynamic_cast<Scope *>(p);
                stages.pop();
            }
            return ptr;
        }

        // This will pop the queue until the last element
        Scope *cut_scope_from(decltype(Token().split_stages()) &stages) {
            bool is_local;
            return cut_scope_from(stages, is_local);
        }

        friend class Keywords;

    protected:
        std::unordered_map<Token, ObjectPtr> objects;
        Report &_report;
        Scope *parent = nullptr;


    public:
        struct Flags {
        private:
            bool colon = false, get_flag = false;
            bool find_no_err = false, set_no_err = false, decl_no_new_scope = false;

            void reset() noexcept {
                colon = get_flag = find_no_err = set_no_err = decl_no_new_scope = make = loc = again = new_decl = false;
            }

            // NOTICE : Call this wherever public flags are used.
            void get(const Scope *scope) noexcept {
                if (!get_flag) {
                    while (scope != nullptr) {
                        make |= scope->flags.make;
                        loc |= scope->flags.loc;
                        again |= scope->flags.again;
                        new_decl != scope->flags.again;
                        scope = scope->parent;
                    }
                    get_flag = true;
                }
            }

            friend class Scope;

        public:
            bool make = false, loc = false, again = false, new_decl = false, returned = false;
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

        ObjectPtr &find(const Token &name, bool &is_local) {
            auto stages = name.split_stages();
            return cut_scope_from(stages, is_local)->find_no_stage(Token{stages.front()}, is_local);
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
            find(token)->exit(_report);
        }

        void erase_object(const Token &token) {
            bool is_local;
            auto stages = token.split_stages();
            cut_scope_from(stages)->erase_no_stage(Token{stages.front()});
        }

        void try_exit_erase_object(const Token &token) {
            if (objects.find(token) != objects.end()) {
                if (objects.at(token) != nullptr)
                    exit_object(token);
                erase_object(token);
            }
        }

        void set_object(const Token &token, const ObjectPtr &object) {
            flags.get(parent);
            bool is_local;
            auto &f = find<true>(token, is_local);
            if (f == null_obj || !is_local && flags.loc) {
                if (!(flags.set_no_err || flags.loc || token.reserved()))
                    _report.report(Report_Undeclared,
                                   "Setting name \"" + std::string(token) +
                                   "\", which is not declared. Unexpected things may happen.");
                try_exit_erase_object(token);
                if (declare_object<true>(token))
                    find<false>(token) = object;
            } else f = object;
        }

        template<bool cancel_warn>
        typename std::enable_if<cancel_warn, void>::type set_object(const Token &token, const ObjectPtr &object) {
            flags.set_no_err = true;
            set_object(token, object);
            flags.set_no_err = false;
        }

        template<bool cancel_warn>
        typename std::enable_if<!cancel_warn, void>::type set_object(const Token &token, const ObjectPtr &object) {
            set_object(token, object);
        }

        // Declares a with-or-without-colon token and return whether the operation is successful.
        bool declare_object(const Token &token) {
            auto stages = token.split_stages();
            bool is_local;
            Scope *ptr = this;
            while (stages.size() != 1) {
                Token name = Token{stages.front()};
                auto p = ptr->find_no_stage(name, is_local).get();
                if (p == nullptr) {
                    if (flags.decl_no_new_scope) {
                        _report.report(Report_Undeclared, "Scope piece \"" + stages.front() + "\" is found nullptr.");
                        return false;
                    }
                } else if (p == null_obj.get()) {
                    if (flags.decl_no_new_scope) {
                        _report.report(Report_Undeclared, "Scope piece \"" + stages.front() + "\" is not found.");
                        return false;
                    } else
                        declare_no_stage(name);
                } else if (!p->is_scope())
                    _report.report(Report_TypeErr,
                                   "Colon shortcuts must prefix with scopes. Not \"" + stages.front() +
                                   "\".");
                else
                    ptr = dynamic_cast<Scope *>(p);
                stages.pop();
            }
            ptr->declare_no_stage(Token{stages.front()});
            return true;
        }

        template<bool no_new>
        typename std::enable_if<no_new, bool>::type declare_object(const Token &token) {
            flags.decl_no_new_scope = true;
            bool result = declare_object(token);
            flags.decl_no_new_scope = false;
            return result;
        }

        template<bool no_new>
        typename std::enable_if<!no_new, bool>::type declare_object(const Token &token) {
            return declare_object(token);
        }

        void copy_objects_from(Scope *scope) {
            for (auto &object: scope->objects)
                set_object<true>(object.first, object.second);
        }

        void put(const Token &token, Report &report) override {
            Token tmp;
            if (flags.returned) return;
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

        [[nodiscard]] constexpr bool is_scope() const noexcept final {
            return true;
        }

        Report &get_report() noexcept {
            return _report;
        }

        void temporary_exit() noexcept {
            flags.returned = false;
        }
    };

    static std::unordered_map<std::string, std::shared_ptr<Scope>> pups_modules;

    ObjectPtr make_scope(const Token &name, const fpath &path, Scope *parent, Report &report,
                         const std::unordered_map<Token, Token> &args = {});

    ObjectPtr make_scope(const Token &name, const Token &block, Scope *parent, Report &report,
                         const std::unordered_map<Token, Token> &args = {});

    ObjectPtr make_free_scope(const fpath &path, Scope *parent, Report &report);

    std::shared_ptr<Scope> &find_module(const fpath &path) {
        return pups_modules.at(absolute(path).string());
    }

    void add_module(const fpath &path, const std::shared_ptr<Scope> &new_module) {
        pups_modules.insert({absolute(path).string(), new_module});
    }

    inline void add_scope(const Token &name, std::shared_ptr<Scope> &scope, Scope *parent) {
        if (name.empty())
            parent->copy_objects_from(scope.get());
        else
            parent->set_object(name, scope);
    }
}

#endif //POURUPSCRIPT_SCOPE_HPP
