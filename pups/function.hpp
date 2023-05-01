//
// Created by Dogs-Cute on 5/1/2023.
//

#ifndef POURUPSCRIPTINTERP_FUNCTION_HPP
#define POURUPSCRIPTINTERP_FUNCTION_HPP

#include "stdtype.hpp"

namespace PUPS {

    class INST_Function : public InstanceBase {
        using TypeSpec = std::vector<Cnt>;
        using Parameter = std::pair<TypeSpec, Token>;
        using ParameterList = std::vector<Parameter>;
        using ArgumentList = std::unordered_map<Token, Token>;
        static constexpr const ObjectBase::Cnt any_type = 0;

        friend class TP_Function;

        static constexpr const cstr determined = "=";

        ParameterList parameters;
        Token body;

        std::stack<ArgumentList> arguments;
        std::stack<std::list<Token>> to_gives;

        void emplace_to_give() {
            to_gives.emplace();
            for (auto &pair: parameters)
                to_gives.top().push_back(pair.second);
        }

        void reset() {
            arguments.top().clear();
            to_gives.pop();
            emplace_to_give();
        }

        // Call with care. Does not check if the arguments are lexically right.
        bool check_args(Scope *scope, Report &report, const ArgumentList &argument) {
            for (auto &parameter: parameters) {
                auto &name = argument.at(parameter.second);
                auto &obj = scope->find<true>(name);
                if (obj == nullptr || !check(parameter.first, obj)) {
                    report.report(Report_TypeErr,
                                  "Argument \"" + std::string(parameter.second) +
                                  "\" get object " + get_ptr_string(obj) +
                                  ", which does not fit in the type.");
                    return false;
                }
            }
            return true;
        }


    protected:
        static bool check(const TypeSpec &type_spec, const ObjectPtr &arg) noexcept {
            return std::ranges::any_of(type_spec.cbegin(), type_spec.cend(),
                                       [&arg](Cnt x) -> bool { return x == 0 || x == arg->type(); });
        }

        ObjectPtr call(Scope *scope, Report &report) {
            auto &argument = arguments.top();
            arguments.emplace();
            emplace_to_give();
            Token name = next_tag();

            if (!check_args(scope, report, argument)) return null_obj;
            auto new_scope = make_scope(name, body, scope, report, argument);

            ObjectPtr result = std::static_pointer_cast<Scope>(new_scope)->find<true>(Token{RETURN_V});
            scope->try_exit_erase_object(name);
            arguments.pop();
            to_gives.pop();
            return result;
        }

    public:

        INST_Function(Cnt type, ParameterList parameters, Token body) :
                InstanceBase(type), parameters(std::move(parameters)), body(std::move(body)) {
            arguments.emplace();
            emplace_to_give();
        }

        void inst_put(const Token &token, Report &report) final {
            auto &argument = arguments.top();
            auto &to_give = to_gives.top();
            std::string arg = token.str_dependent_no_space();
            auto det = arg.find(determined);
            if (det == std::string::npos) {
                argument.insert({to_give.front(), Token{arg}});
                if (to_give.empty())
                    report.report(Report_IncorrectArguments,
                                  "Too many implicit arguments given. Argument \"" + arg + "\" skipped.");
                else
                    to_give.pop_front();
            } else {
                std::string name = arg.substr(0, det);
                arg = arg.substr(det + strlen(determined));
                for (auto iter = to_give.begin(); iter != to_give.end(); iter++) {
                    if (*iter == name) {
                        argument.insert({*iter, Token{arg}});
                        to_give.erase(iter);
                        return;
                    }
                }
                report.report(Report_IncorrectArguments,
                              "Could not find explicit argument \"" + name + "\" in function call.");
            }
        }

        ObjectPtr inst_ends(PUPS::Scope *scope, PUPS::Report &report) final {
            auto &to_give = to_gives.top();
            if (!to_give.empty()) {
                report.report(Report_IncorrectArguments,
                              "Arguments not enough in function call (" + std::to_string(to_give.size()) +
                              " remaining). Function call skipped.");
                return null_obj;
            }
            auto result = call(scope, report);
            if (arguments.size() == 1)
                reset();
            return result;
        }

        [[nodiscard]] ObjectPtr copy() const noexcept override {
            return ObjectPtr{new INST_Function{type(), parameters, body}};
        }
    };

    class TP_Function : public TypeBase {
        static constexpr const char *const type_symbol = "@";
        using TypeSpec = INST_Function::TypeSpec;
        using ParameterList = INST_Function::ParameterList;

        static void get_spec(Scope *scope, Report &report, TypeSpec &spec, const std::string &type_str) {
            auto types = split_spaces(type_str);
            while (!types.empty()) {
                auto object = scope->find(Token{types.front()});
                if (object->is_type())
                    spec.push_back(std::static_pointer_cast<TypeBase>(object)->get_type());
                else
                    report.report(Report_TypeErr,
                                  "On function initialization, token \"" + types.front() +
                                  "\" does not refer to a type.");
                types.pop();
            }
        }

        static void add_parameters(const TypeSpec &spec, ParameterList &parameters,
                                   const std::string &arg_str) {
            auto args = split_spaces(arg_str);
            while (!args.empty()) {
                parameters.emplace_back(spec, args.front());
                args.pop();
            }
        }

    protected:
        ObjectPtr construct(Scope *scope, Report &report, std::queue<Token> &names,
                            std::queue<Token> &args) override {
            ParameterList parameters;
            while (args.size() != 1) {
                std::string arg = args.front().str_dependent();
                auto type_index = arg.find(type_symbol);
                TypeSpec spec;
                if (type_index == std::string::npos)
                    spec = {0};
                else {
                    std::string type = arg.substr(0, type_index);
                    arg = arg.substr(type_index + strlen(type_symbol));

                    get_spec(scope, report, spec, type);
                }

                add_parameters(spec, parameters, arg);
                args.pop();
            }
            if (!args.front().is_long())
                report.report(Report_WrongToken, "The final argument to construct function must be long, not \"" +
                                                 std::string(args.front()) + "\".");
            else {
                ObjectPtr func = ObjectPtr{new INST_Function{cnt, parameters, args.front()}};
                while (!names.empty()) {
                    scope->set_object(names.front(), func);
                    names.pop();
                }
            }
            args.pop();
            return null_obj;
        }

        [[nodiscard]] constexpr bool require_assign() const noexcept final {
            return false;
        }
    };

    using BuiltinFunction = ObjectPtr (*)(std::queue<ObjectPtr *> &, Report &);
    std::unordered_map<Token, ObjectPtr> builtins;

    class GlobalFunction final : public ObjectBase {
        BuiltinFunction func;
        std::stack<std::queue<Token>> args;
    public:

        explicit GlobalFunction(BuiltinFunction func) : func(func) {
            args.emplace();
        }

        void put(const Token &token, Report &report) override {
            args.top().push(token);
        }

        ObjectPtr ends(Scope *scope, Report &report) override {
            std::queue<ObjectPtr *> objects;
            while (!args.top().empty()) {
                objects.push(&scope->find(args.top().front()));
                args.top().pop();
            }
            args.emplace();
            auto result = func(objects, report);
            args.pop();
            if (args.size() == 1)
                while (!args.top().empty()) args.top().pop();
            return result;
        }

        void exit(Report &report) override {}

        [[nodiscard]] constexpr bool can_delete() const noexcept override {
            return false;
        }
    };

    void add_to_builtins(const Token &name, BuiltinFunction func) {
        builtins.insert({add_builtin_mark(name.str()), std::make_shared<GlobalFunction>(func)});
    }

#define MAKE_BUILTIN(fn_name) inline ObjectPtr fn_name(std::queue<ObjectPtr *> &args, Report &report)
}

#endif //POURUPSCRIPTINTERP_FUNCTION_HPP
