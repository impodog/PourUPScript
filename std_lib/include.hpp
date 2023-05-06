//
// Created by Dogs-Cute on 5/1/2023.
//

#ifndef POURUPSCRIPTINTERP_INCLUDE_HPP
#define POURUPSCRIPTINTERP_INCLUDE_HPP

#include "../pups.h"
#include "lib_const.hpp"
#include "lib_math.hpp"
#include "lib_util.hpp"
#include "lib_random.hpp"

namespace PUPS {

    // Adds all std libraries.
    void INCLUDE_ALL() {
        Std_Const::INCLUDE();
        Std_Util::INCLUDE();
        Std_Math::INCLUDE();
        Std_Random::INCLUDE();
    }
}

#endif //POURUPSCRIPTINTERP_INCLUDE_HPP
