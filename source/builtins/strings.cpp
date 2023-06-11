//
// Created by impodog on 6/3/2023.
//

#include "strings.h"

#include <utility>

namespace pups::library::builtins::strings {
    constexpr const char *STRING_TYPE_NAME = "s";

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
        return "s";
    }

#define s_func(not_op) [](FunctionArgs &args, Map *map) -> ObjectPtr {\
    const std::string *str = nullptr;\
    while (!args.empty()) {\
        auto ptr = std::dynamic_pointer_cast<String>(*args.front());\
        if (ptr) {\
            if (str) {\
                if (ptr->data() not_op *str)\
                    return numbers::False;\
            } else\
                str = &ptr->data();\
        } else\
            map->throw_error(std::make_shared<TypeError>("Incorrect call of type on STR.eq."));\
        args.pop();\
    }\
    return numbers::True;\
}

    void init(Constants &constants) {
        static const auto add_s_func = [&constants](const std::string &name, const FunctionCore &core) {
            constants.add(template_name(name, {STRING_TYPE_NAME}), std::make_shared<Function>(core));
        };
        add_s_func("gt", s_func(>));
        add_s_func("ge", s_func(>=));
        add_s_func("lt", s_func(<));
        add_s_func("le", s_func(<=));
        add_s_func("eq", s_func(!=));
        add_s_func("ne", s_func(!=));
    }
}
