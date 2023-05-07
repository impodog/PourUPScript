//
// Created by Dogs-Cute on 5/7/2023.
//

#ifndef POURUPSCRIPTINTERP_LIB_DYNAMIC_HPP
#define POURUPSCRIPTINTERP_LIB_DYNAMIC_HPP

#include "../pups.h"

namespace PUPS::Std_Dynamic {
    MAKE_BUILTIN(DYNAMIC_run) {
        if (args.empty()) {
            report.report("DYNAMIC_run", "No arguments.");
            return null_obj;
        }
        std::string str;
        while (!args.empty()) {
            auto v = (*args.front())->get_ev();
            if (v.type != EvalResult::type_str) {
                report.report("DYNAMIC_run", "Incorrect type.");
                return null_obj;
            }
            str.append(*v.result.r_str + " ");
            args.pop();
        }
        Token name = next_tag();
        return create_scope(name, str, scope, report);
    }

    void INCLUDE() {
        add_to_builtins("DYNAMIC_run", DYNAMIC_run);
    }
}

#endif //POURUPSCRIPTINTERP_LIB_DYNAMIC_HPP
