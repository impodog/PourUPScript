//
// Created by Dogs-Cute on 5/27/2023.
//

#ifndef PUPS_LIB_BUILTINS_H
#define PUPS_LIB_BUILTINS_H

#include "function.h"
#include "numbers.h"
#include "inline_func.h"
#include "keyword_func.h"
#include "strings.h"
#include "map_open.h"
#include "qualifiers.h"
#include "builtin_func.h"
#include "reference.h"
#include "containers/containers.h"
#include "ranges/ranges.h"


namespace pups::library::builtins {
    void init(Constants &constants);
}

#endif //PUPS_LIB_BUILTINS_H
