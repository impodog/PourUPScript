//
// Created by impodog on 6/3/2023.
//

#ifndef PUPS_LIB_TESTS_STRINGS_H
#define PUPS_LIB_TESTS_STRINGS_H

#include "function.h"
#include "numbers.h"

namespace pups::library::builtins::strings {
    using namespace function;

    class String : public Object {
        std::string m_data;
        ObjectPtr eq_function = std::make_shared<Function>([this](FunctionArgs &args, Map *map) -> ObjectPtr {
            if (args.size() != 1)
                map->throw_error(std::make_shared<ArgumentError>("String.eq requires one single argument"));
            else {
                auto ptr = std::dynamic_pointer_cast<String>(*args.front());
                if (ptr)
                    return this->data() == ptr->data() ? numbers::True : numbers::False;
                else
                    map->throw_error(std::make_shared<ArgumentError>("String.eq requires a string argument"));
            }
            return pending;
        });
    public:
        explicit String(std::string data);

        ObjectPtr put(ObjectPtr &object, Map *map) override;


        [[nodiscard]] std::string str() const noexcept override;

        [[nodiscard]] std::string &data() noexcept;

        [[nodiscard]] const std::string &data() const noexcept;

        [[nodiscard]] std::string type_name() const noexcept override;

        [[nodiscard]] size_t hash() const noexcept override;

        [[nodiscard]] size_t equal(const ObjectPtr &object) const noexcept override;
    };

    void init(Constants &constants);
}

#endif //PUPS_LIB_TESTS_STRINGS_H
