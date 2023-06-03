//
// Created by impodog on 6/3/2023.
//

#ifndef PUPS_LIB_TESTS_MAP_OPEN_H
#define PUPS_LIB_TESTS_MAP_OPEN_H

#include "function.h"

namespace pups::library::builtins::map_open {
    using namespace function;

    class MapOpen : public Function {
    public:
        MapOpen();
    };

    void init(Constants &constants);
}

#endif //PUPS_LIB_TESTS_MAP_OPEN_H
