//
// Created by impodog on 6/3/2023.
//

#ifndef PUPS_LIB_STRINGS_H
#define PUPS_LIB_STRINGS_H

#include "../function.h"
#include "numbers.h"

namespace pups::library::builtins::strings {
    using namespace function;

    class String : public HasMethods {
        std::string m_data;
        ObjectMap used_functions;
    public:
        explicit String(std::string data);

        ObjectPtr put(ObjectPtr &object, Map *map) override;

        [[nodiscard]] std::string str() const noexcept override;

        std::string repr() const noexcept override;

        FunctionCore get_method(const Id &name) override;


        [[nodiscard]] std::string &data() noexcept;

        [[nodiscard]] const std::string &data() const noexcept;

        [[nodiscard]] std::string type_name() const noexcept override;

        [[nodiscard]] size_t hash() noexcept override;

        [[nodiscard]] bool equal(ObjectPtr &object) noexcept override;

        int compare(ObjectPtr &object, Map *map) noexcept override;
    };

    using StringFuncCore = std::function<ObjectPtr(String &, FunctionArgs &, Map *)>;
    using StringFuncMap = IdMap<StringFuncCore>;
    extern const StringFuncMap string_functions;

    std::string str_of(FunctionArgs &args);

    std::string repr_of(FunctionArgs &args);

    void init(Constants &constants);
}

#endif //PUPS_LIB_STRINGS_H
