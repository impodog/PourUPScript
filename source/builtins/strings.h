//
// Created by impodog on 6/3/2023.
//

#ifndef PUPS_LIB_TESTS_STRINGS_H
#define PUPS_LIB_TESTS_STRINGS_H

#include "function.h"

namespace pups::library::builtins::strings {

    class String : public Object {
        std::string m_data;
    public:
        explicit String(std::string data);

        ObjectPtr put(ObjectPtr &object, Map *map) override;

        [[nodiscard]] std::string str() const noexcept override;

        [[nodiscard]] const std::string &data() const noexcept;
    };

    void init(Constants &constants);
}

#endif //PUPS_LIB_TESTS_STRINGS_H
