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
        while (!args.empty()) {
            if (args.front() == nullptr)
                return true_obj;
            args.pop();
        }
        return false_obj;
    }

    MAKE_BUILTIN(UTIL_to_boolean) {
        if (args.size() != 1) {
            report.report("UTIL_to_boolean",
                          "Conversion should get one argument only. Statement skipped(NULL is returned.).");
            return null_obj;
        }
        return std::make_shared<INST_Byte>(TypeCodes::Byte, (*args.front())->to_condition());
    }

    MAKE_BUILTIN(UTIL_to_not_boolean) {
        if (args.size() != 1) {
            report.report("UTIL_to_not_boolean",
                          "Conversion should get one argument only. Statement skipped(NULL is returned.).");
            return null_obj;
        }
        return std::make_shared<INST_Byte>(TypeCodes::Byte, !(*args.front())->to_condition());
    }

    MAKE_BUILTIN(UTIL_to_str) {
        std::string result;
        while (!args.empty()) {
            result.append((*args.front())->to_string());
            args.pop();
        }
        return std::make_shared<INST_Str>(TypeCodes::Str, result);
    }

    inline void INCLUDE_UTIL() {
        add_to_builtins("UTIL_same", UTIL_same);
        add_to_builtins("UTIL_any_null", UTIL_any_null);
        add_to_builtins("UTIL_invert", UTIL_any_null);
        add_to_builtins("UTIL_to_boolean", UTIL_to_boolean);
        add_to_builtins("UTIL_to_not_boolean", UTIL_to_not_boolean);
        add_to_builtins("UTIL_to_str", UTIL_to_str);
    }
}

#endif //POURUPSCRIPTINTERP_LIB_UTIL_HPP
