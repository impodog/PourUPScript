//
// Created by Dogs-Cute on 5/27/2023.
//

#ifndef PUPS_LIB_BUILTINS_H
#define PUPS_LIB_BUILTINS_H

#include "function.h"
#include "types/numbers.h"
#include "keywords/inline_func.h"
#include "keywords/keyword_func.h"
#include "types/strings.h"
#include "keywords/map_open.h"
#include "keywords/qualifiers.h"
#include "keywords/builtin_func.h"
#include "keywords/compare.h"
#include "types/reference.h"
#include "containers/containers.h"
#include "ranges/ranges.h"
#include "types/typing.h"
#include "keywords/inline_typing.h"
#include "debugger/debug_info.h"
#include "structural/enumerate.h"
#include "structural/match_case.h"


namespace pups::library::builtins {
    void init(Constants &constants);
}

#endif //PUPS_LIB_BUILTINS_H
