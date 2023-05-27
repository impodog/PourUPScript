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

namespace pups::library {
    using std::string, std::ifstream, std::ofstream, std::queue, std::vector, std::unordered_map, std::shared_ptr,
            std::filesystem::path;

    constexpr const char *const PUPS_LIB_VERSION = "0.0.1";
    constexpr const char segment_symbol = '%';
}

#endif //PUPS_EZN_CONST_H