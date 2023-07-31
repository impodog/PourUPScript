//
// Created by impodog on 7/31/2023.
//

#ifndef PUPS_LIB_ENUMERATE_H
#define PUPS_LIB_ENUMERATE_H

#include "../function.h"
#include "../types/numbers.h"

namespace pups::library::builtins::enumerate {
    using namespace function;

    class Enumerate : public Map {
    protected:
        pups_int enumerate = 0;

    public:
        explicit Enumerate(Map *parent_map);

        ObjectPtr put(ObjectPtr &object, Map *map) override;
    };

    class EnumerateInit : public Function {
    public:
        EnumerateInit();
    };

    void init(Constants &constants);
}

#endif //PUPS_LIB_ENUMERATE_H
