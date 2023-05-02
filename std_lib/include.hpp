//
// Created by Dogs-Cute on 5/1/2023.
//

#ifndef POURUPSCRIPTINTERP_INCLUDE_HPP
#define POURUPSCRIPTINTERP_INCLUDE_HPP

#include "../pups.h"
#include "lib_const.hpp"
#include "lib_math.hpp"
#include "lib_util.hpp"

namespace PUPS {

    // Adds all std libraries.
    void INCLUDE_ALL() {
        INCLUDE_CONST();
        INCLUDE_MATH();
        INCLUDE_UTIL();
    }
}

#endif //POURUPSCRIPTINTERP_INCLUDE_HPP
