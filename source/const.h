//
// Created by Dogs-Cute on 5/27/2023.
//

#ifndef PUPS_EZN_CONST_H
#define PUPS_EZN_CONST_H

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

namespace pups::library {
    using std::filesystem::path;
    constexpr const char *const PUPS_LIB_VERSION = "0.4.0";
    constexpr const char segment_symbol = '%';
    constexpr const char *RESERVED = "_R_";
}

#endif //PUPS_EZN_CONST_H
