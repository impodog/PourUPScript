//
// Created by impodog on 6/3/2023.
//

#ifndef PUPS_LIB_MAP_OPEN_H
#define PUPS_LIB_MAP_OPEN_H

#include "../function.h"

namespace pups::library::builtins::map_open {
    using namespace function;

    class MapOpen : public Function {
    public:
        MapOpen();
    };

    class ModuleOpen : public Function {
    public:
        ModuleOpen();
    };

    class MapEnter : public Function {
    public:
        MapEnter();
    };

    class Catch : public Function {
    public:
        Catch();
    };

    void init(Constants &constants);

    void quit();

    void set_module_path(path path);
}

#endif //PUPS_LIB_MAP_OPEN_H
