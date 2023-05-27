//
// Created by Dogs-Cute on 5/27/2023.
//

#ifndef PUPS_LIB_TESTS_FUNCTION_H
#define PUPS_LIB_TESTS_FUNCTION_H

#include "../map.h"

namespace pups::library::builtins {
    using FunctionParameters = std::vector<Id>;

    class Function : public Object {
    protected:
        IdFile m_idFile;
        FunctionParameters parameters;
    public:
        Function() = default;

        explicit Function(IdFile idFile); //todo

        ObjectPtr put(ObjectPtr &object, Map *map) override;

        ObjectPtr end_of_line(Map *map) override;
    };

    void init_function();
}

#endif //PUPS_LIB_TESTS_FUNCTION_H
