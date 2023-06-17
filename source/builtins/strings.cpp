//
// Created by impodog on 6/3/2023.
//

#include "strings.h"

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

#define s_func(op) [](FunctionArgs &args, Map *map) -> ObjectPtr {\
    const std::string *str = nullptr;\
    while (!args.empty()) {\
        auto ptr = std::dynamic_pointer_cast<String>(*args.front());\
        if (ptr) {\
            if (str) {\
                if (!(*str op ptr->data()))\
                    return numbers::False;\
            } else\
                str = &ptr->data();\
        } else\
            map->throw_error(std::make_shared<TypeError>("Incorrect call of type on STR.eq."));\
        args.pop();\
    }\
    return numbers::True;\
}

    template<typename Arithmetic>
    ObjectPtr string_to(FunctionArgs &args, Map *map) {
        if (args.size() == 1) {
            auto ptr = std::dynamic_pointer_cast<String>(*args.front());
            if (ptr) {
                Arithmetic x;
                try {
                    if constexpr (std::is_same<Arithmetic, int>::value)
                        x = std::stoi(ptr->data());
                    else if constexpr (std::is_same<Arithmetic, float>::value)
                        x = std::stof(ptr->data());
                    else if constexpr (std::is_same<Arithmetic, bool>::value)
                        x = ptr->data() == "true";
                } catch (const std::invalid_argument &) {
                    map->throw_error(std::make_shared<ValueError>(
                            "Invalid value \"" + ptr->data() + "\" for string conversion."));
                    return pending;
                }
                return std::make_shared<numbers::NumType<Arithmetic>>(x);
            } else
                map->throw_error(std::make_shared<TypeError>("String conversion requires a string."));
        } else
            map->throw_error(
                    std::make_shared<ArgumentError>(
                            "String conversion method does not require addition arguments."));
        return pending;
    }

    FunctionCore string_function(const std::function<std::string(const std::string &, const std::string &)> &core) {
        return [core](FunctionArgs &args, Map *map) -> ObjectPtr {
            if (args.size() != 2)
                map->throw_error(std::make_shared<ArgumentError>("String operators requires two only arguments."));
            else {
                auto lhs = std::dynamic_pointer_cast<String>(*args.front());
                args.pop();
                auto rhs = std::dynamic_pointer_cast<String>(*args.front());
                args.pop();
                if (lhs && rhs)
                    return std::make_shared<String>(core(lhs->data(), rhs->data()));
                else
                    map->throw_error(std::make_shared<TypeError>("String operators require two string arguments."));
            }
            return pending;
        };
    }

    ObjectPtr repr_of(FunctionArgs &args, Map *map) {
        std::string result;
        while (!args.empty()) {
            result.append(args.front()->get()->str());
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

    Id id_repr_of{"", "repr_of"}, id_typename_of{"", "typename_of"};

    void init(Constants &constants) {
        static const auto add_s_func = [&constants](const std::string &name, const FunctionCore &core) {
            constants.add(template_name(name, {name_string}), std::make_shared<Function>(core));
        };
        constants.add(id_repr_of, std::make_shared<Function>(repr_of));
        constants.add(id_typename_of, std::make_shared<Function>(typename_of));
        add_s_func("gt", s_func(>));
        add_s_func("ge", s_func(>=));
        add_s_func("lt", s_func(<));
        add_s_func("le", s_func(<=));
        add_s_func("eq", s_func(==));
        add_s_func("ne", s_func(!=));
        add_s_func("toi", string_to<int>);
        add_s_func("tof", string_to<float>);
        add_s_func("bool", string_to<bool>);
        add_s_func("add", string_function([](const std::string &lhs, const std::string &rhs) -> std::string {
            return lhs + rhs;
        }));
    }
}
