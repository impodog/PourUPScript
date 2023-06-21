//
// Created by impodog on 6/18/2023.
//

#ifndef PUPS_LIB_TESTS_STD_H
#define PUPS_LIB_TESTS_STD_H

#include "../../pups.h"

namespace pups::modules::pups_std {
    constexpr pups::library::cstr STD_NAME = "__std_";

    void init(Constants &constants);

    Id get_std_func_name(const std::string &name);
}

#endif //PUPS_LIB_TESTS_STD_H
