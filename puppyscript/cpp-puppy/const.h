//
// Created by impodog on 7/30/2023.
//

#ifndef CPP_PUPPY_CONST_H
#define CPP_PUPPY_CONST_H

#define PUPPY_VERSION "0.0.1"

#include <string>
#include <filesystem>
#include <regex>
#include <list>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <memory>
#include <utility>
#include <cctype>
#include <iostream>
#include <fstream>
#include <stack>
#include <numeric>
#include <tuple>

#define INT long long
#define FLOAT double
#define BOOL bool

#define WITH_STMT "with"
#define DEBUG_INFO_STMT "DEBUG_INFO"

#define SPECIAL_PREFIX "__spec_"
#define ELLIPSIS "..."
#define ELLIPSIS_SIZE 3

#define WORD R"([\w\d\.\&\$\~]+)"
#define FLAGS R"(([\w\s]*\s+)?)"
#define INDENT R"([^\S\n]*)"

#define CONST_SUFFIX ".con"

#define NPOS ((size_t) -1)

#define TEMP_ID "_R_TMP" // It is deprecated

using std::filesystem::path;

const std::regex RE_WORD(WORD),
        RE_FLAGS(FLAGS),
        RE_INDENT(INDENT);
enum Rule {
    _,
    MODULE,
    ALLOW_NO_COLON
};
using RuleTable = std::unordered_map<Rule, int>;
using RuleNames = std::unordered_map<std::string, Rule>;

#endif //CPP_PUPPY_CONST_H
