//
// Created by Dogs-Cute on 4/15/2023.
//

#ifndef POURUPSCRIPT_MACROS_H
#define POURUPSCRIPT_MACROS_H

#include "../cpp_lib/cpp_lib.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <ranges>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <stack>
#include <list>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <queue>
#include <utility>
#include <memory>
#include <cmath>


#define FN_ALIAS_SPEC(SPECS, NEW_NAME, ...)                                     \
  template <typename... Args>                                                   \
  SPECS auto NEW_NAME(Args &&... __args__)                                          \
    noexcept(noexcept(__VA_ARGS__(std::forward<Args>(__args__)...)))                \
    -> decltype(__VA_ARGS__(std::forward<Args>(__args__)...)) {                     \
    return __VA_ARGS__(std::forward<Args>(__args__)...);                            \
  }

#define FN_ALIAS(NEW_NAME, ...)                                                 \
  FN_ALIAS_SPEC(inline, NEW_NAME, __VA_ARGS__)

#define max_m(x, y) (x > y ? x : y)
#define min_m(x, y) (x < y ? x : y)

#ifdef WIN32

#include "windows.h"

#endif // WIN32

namespace PUPS {
    using cppstr = const std::string &;
    using cstr = const char *;
    using fpath = std::filesystem::path;

    constexpr const cstr version = "0.3.3";

    constexpr const cstr builtin_mark = "__builtin_";

    constexpr const cstr std_path = "std",
            DECL = "decl",
            REF = "ref",
            PRINT = "print",
            SET = "set",
            LOC = "local",
            SCO = "scope",
            NOTHING = "nothing",
            INCL = "incl",
            COMM = "comment",
            NULL_OBJ = "null",
            MAKE = "make",
            AGAIN = "again",
            ADD_PATH = "add_path",
            NEW = "new",
            REMOVE = "rmv",
            RETURN = "ret",
            IF = "if",
            WHILE = "while",
            DO = "do",
            NUMBER = "number",


            INT = "int",
            FLOAT = "flo",
            STR = "str",
            BYTE = "byte",
            SCHAR = "schar",
            FUNCTION = "fn",
            ANYT = "anyt",
            TYPET = "typet",
            NULLT = "nullt",
            LAMBDA = "lambda",

            RETURN_V = "__return_v__",
            FALSE_OBJ = "false",
            TRUE_OBJ = "true";
    constexpr const char call_symbol = 'C';

    bool keepTemporary = false;

}

#endif //POURUPSCRIPT_MACROS_H
