//
// Created by impodog on 6/3/2023.
//

#include "strings.h"
#include "containers/containers.h"

#include <utility>

namespace pups::library::builtins::strings {
    constexpr const char *name_string = "str";

    String::String(std::string data) : m_data(std::move(data)) {

    }

    ObjectPtr String::put(ObjectPtr &object, Map *map) {
        auto ptr = std::dynamic_pointer_cast<String>(object);
        if (ptr) {
            return std::make_shared<String>(m_data + ptr->m_data);
        } else {
            map->throw_error(std::make_shared<TypeError>("Cannot put anything other than string into string."));
            return pending;
        }
    }

    std::string String::str() const noexcept {
        return m_data;
    }

    std::string String::repr() const noexcept {
        return "\"" + m_data + "\"";
    }

    FunctionCore String::get_method(const Id &name) {
        const auto &func = string_functions.at(name);
        return [this, &func](FunctionArgs &args, Map *map) -> ObjectPtr {
            return func(*this, args, map);
        };
    }

    std::string &String::data() noexcept {
        return m_data;
    }

    const std::string &String::data() const noexcept {
        return m_data;
    }

    std::string String::type_name() const noexcept {
        return name_string;
    }

    size_t String::hash() const noexcept {
        return std::hash<std::string>()(m_data);
    }

    size_t String::equal(const ObjectPtr &object) const noexcept {
        auto ptr = std::dynamic_pointer_cast<String>(object);
        return ptr && m_data == ptr->m_data;
    }

    using StringCompare = std::function<bool(std::string &, std::string &)>;

    StringFuncCore str_cmp(const StringCompare &cmp) {
        return [cmp](String &str, FunctionArgs &args, Map *map) -> ObjectPtr {
            auto &str_data = str.data();
            while (!args.empty()) {
                auto ptr = std::dynamic_pointer_cast<String>(*args.front());
                if (ptr) {
                    if (!cmp(str_data, ptr->data()))
                        return numbers::False;
                } else
                    map->throw_error(std::make_shared<TypeError>("Incorrect call of type on String.compare ."));
                args.pop();
            }
            return numbers::True;
        };
    }


    template<typename Arithmetic>
    ObjectPtr string_to(String &str, FunctionArgs &args, Map *map) {
        if (!args.empty())
            map->throw_error(
                    std::make_shared<ArgumentError>(
                            "String conversion method does not require addition arguments."));
        Arithmetic x;
        try {
            if constexpr (std::is_same<Arithmetic, pups_int>::value)
                x = std::stoi(str.data());
            else if constexpr (std::is_same<Arithmetic, pups_float>::value)
                x = std::stof(str.data());
            else if constexpr (std::is_same<Arithmetic, pups_bool>::value)
                x = str.data() == "true";
        } catch (const std::invalid_argument &) {
            map->throw_error(std::make_shared<ValueError>(
                    "Invalid value \"" + str.data() + "\" for string conversion."));
            return pending;
        }
        return std::make_shared<numbers::NumType<Arithmetic>>(x);
    }

    StringFuncCore string_function(const std::function<std::string(const std::string &, const std::string &)> &core) {
        return [core](String &str, FunctionArgs &args, Map *map) -> ObjectPtr {
            if (args.size() != 1)
                map->throw_error(std::make_shared<ArgumentError>("String operators requires one only argument."));
            else {
                auto rhs = std::dynamic_pointer_cast<String>(*args.front());
                args.pop();
                if (rhs)
                    return std::make_shared<String>(core(str.data(), rhs->data()));
                else
                    map->throw_error(std::make_shared<TypeError>("String operators require one string argument."));
            }
            return pending;
        };
    }

    ObjectPtr string_size(String &str, FunctionArgs &args, Map *map) {
        if (!args.empty())
            map->throw_error(std::make_shared<ArgumentError>("String.size requires no arguments."));
        else
            return std::make_shared<numbers::IntType>(str.data().size());
        return pending;
    }

    ObjectPtr string_at(String &str, FunctionArgs &args, Map *map) {
        if (args.size() != 1)
            map->throw_error(std::make_shared<ArgumentError>("String.at requires one only argument."));
        else {
            {
                auto ptr = std::dynamic_pointer_cast<numbers::IntType>(*args.front());
                if (ptr) {
                    try {
                        std::string s;
                        s.push_back(str.data().at(ptr->value));
                        return std::make_shared<String>(s);
                    } catch (const std::out_of_range &) {
                        map->throw_error(std::make_shared<OutOfBoundError>(
                                "String.at is out of bounds with index " + ptr->repr() + " within str " + str.repr() +
                                "."));
                        return pending;
                    }
                }
            }
            {
                auto ptr = std::dynamic_pointer_cast<containers::Pair>(*args.front());
                if (ptr) {
                    auto left = std::dynamic_pointer_cast<numbers::IntType>(ptr->left);
                    auto right = std::dynamic_pointer_cast<numbers::IntType>(ptr->right);
                    if (left && right)
                        return std::make_shared<String>(str.data().substr(left->value, right->value - left->value));
                }
            }
            map->throw_error(std::make_shared<TypeError>(
                    "String.at requires an integer or an integer pair instead of" + args.front()->get()->repr() + '.'));
        }
        return pending;
    }

    ObjectPtr str_of(FunctionArgs &args, Map *map) {
        std::string result;
        while (!args.empty()) {
            result.append(args.front()->get()->str());
            args.pop();
        }
        return std::make_shared<String>(result);
    }

    ObjectPtr repr_of(FunctionArgs &args, Map *map) {
        std::string result;
        while (!args.empty()) {
            result.append(args.front()->get()->repr());
            args.pop();
        }
        return std::make_shared<String>(result);
    }

    ObjectPtr typename_of(FunctionArgs &args, Map *map) {
        std::string result;
        while (!args.empty()) {
            result.append(args.front()->get()->type_name()).push_back('_');
            args.pop();
        }
        return std::make_shared<String>(result.substr(0, result.size() - 1));
    }

    Id id_str_of{"", "str_of"}, id_repr_of{"", "repr_of"}, id_typename_of{"", "typename_of"};
#define STR_CMP(op)str_cmp([](std::string &lhs, std::string &rhs) {return lhs op rhs;})
    const StringFuncMap string_functions = {
            {Id{"", "gt"}, STR_CMP(>)},
            {Id{"", "ge"}, STR_CMP(>=)},
            {Id{"", "lt"}, STR_CMP(<)},
            {Id{"", "le"}, STR_CMP(<=)},
            {Id{"", "eq"}, STR_CMP(==)},
            {Id{"", "ne"}, STR_CMP(!=)},
            {Id{"", "toi"},      string_to<pups_int>},
            {Id{"", "tof"},      string_to<pups_float>},
            {Id{"", "add"},      string_function([](const std::string &lhs, const std::string &rhs) -> std::string {
                return lhs + rhs;
            })},
            {Id{"", "get_size"}, string_size},
            {Id{"", "at"},       string_at},
            {Id{"", "get"},       string_at}
    };
#undef STR_CMP

    void init(Constants &constants) {
        /*
        static const auto add_s_func = [&constants](const std::string &name, const FunctionCore &core) {
            constants.add(template_name(name, {name_string}), std::make_shared<Function>(core));
        };
        */
        constants.add(id_str_of, std::make_shared<Function>(str_of));
        constants.add(id_repr_of, std::make_shared<Function>(repr_of));
        constants.add(id_typename_of, std::make_shared<Function>(typename_of));
    }
}
