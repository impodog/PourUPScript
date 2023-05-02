//
// Created by Dogs-Cute on 5/1/2023.
//

#ifndef POURUPSCRIPTINTERP_LIB_CONST_HPP
#define POURUPSCRIPTINTERP_LIB_CONST_HPP

#include "../pups.h"

namespace PUPS {
    MAKE_BUILTIN(CONST_version) {
        if (!args.empty())
            report.report("CONST_version", "No args should be received.");
        return std::make_shared<INST_Str>(TypeCodes::Str, version);
    }

    void INCLUDE_CONST() {
        add_to_builtins("CONST_version", CONST_version);
    }
}

#endif //POURUPSCRIPTINTERP_LIB_CONST_HPP
