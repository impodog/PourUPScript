//
// Created by Dogs-Cute on 5/1/2023.
//

#ifndef POURUPSCRIPTINTERP_LIB_UTIL_HPP
#define POURUPSCRIPTINTERP_LIB_UTIL_HPP

#include "../pups.h"

namespace PUPS {
    MAKE_BUILTIN(UTIL_same) {
        ObjectPtr ptr;
        while (!args.empty()) {
            if (ptr) {
                if (*args.front() != ptr) return false_obj;
            } else
                ptr = *args.front();
            args.pop();
        }
        return true_obj;
    }

    MAKE_BUILTIN(UTIL_any_null) {
        ObjectPtr ptr;
        while (!args.empty()) {
            if (ptr == nullptr)
                return true_obj;
            args.pop();
        }
        return false_obj;
    }

    inline void INCLUDE_UTIL() {
        add_to_builtins(Token{"UTIL_same"}, UTIL_same);
        add_to_builtins(Token{"UTIL_any_null"}, UTIL_any_null);
    }
}

#endif //POURUPSCRIPTINTERP_LIB_UTIL_HPP
