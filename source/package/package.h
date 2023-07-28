//
// Created by impodog on 7/27/2023.
//

#ifndef PUPS_LIB_PACKAGE_H
#define PUPS_LIB_PACKAGE_H

#include "../control.h"
#include "../builtins/builtins.h"

namespace pups::library::package {
    using namespace builtins;
    using namespace function;


    using PackageHandler = Constants;

    using PUPS_InitFunction = void (*)(PackageHandler &);

    void init(Constants &constants);

    void quit();
}

#endif //PUPS_LIB_PACKAGE_H
