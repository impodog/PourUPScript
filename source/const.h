//
// Created by Dogs-Cute on 5/27/2023.
//

#ifndef PUPS_CONST_H
#define PUPS_CONST_H

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <queue>
#include <stack>
#include <vector>
#include <unordered_map>
#include <memory>
#include <utility>
#include <valarray>
#include <functional>
#include <sstream>

#define sign_of(x) (x>0 ? 1 : (x<0 ? -1 : 0))
#define sign_sub(x, y) (x>y ? 1 : (x<y ? -1 : 0))

namespace pups::library {
    using std::filesystem::path;
    using cstr = const char *;

    constexpr cstr
            PUPS_LIB_VERSION = "0.8.1",
            RESERVED = "_R_",
            MODULE_LINK = "_ML_";
    constexpr const char segment_symbol = '%';

    using pups_int = long long;
    using pups_float = double;
    using pups_bool = bool;
}

#endif //PUPS_CONST_H
