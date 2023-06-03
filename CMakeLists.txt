cmake_minimum_required(VERSION 3.25)
project(pups_Lib)

set(CMAKE_CXX_STANDARD 17)

add_library(pups_Lib source/const.h source/syntax.cpp source/syntax.h source/object.cpp source/object.h source/map.cpp source/map.h source/control.cpp source/control.h source/error.cpp source/error.h source/builtins/numbers.cpp source/builtins/numbers.h source/builtins/function.cpp source/builtins/function.h source/builtins/builtins.h source/builtins/builtins.cpp source/builtins/inline_func.cpp source/builtins/inline_func.h source/builtins/keyword_func.cpp source/builtins/keyword_func.h)

project(pups_Lib_Tests)

add_executable(pups_Lib_Tests source/const.h source/syntax.cpp source/syntax.h test.cpp source/object.cpp source/object.h source/map.cpp source/map.h source/control.cpp source/control.h source/error.cpp source/error.h source/builtins/numbers.cpp source/builtins/numbers.h source/builtins/function.cpp source/builtins/function.h source/builtins/builtins.h source/builtins/builtins.cpp source/builtins/inline_func.cpp source/builtins/inline_func.h source/builtins/inline_func.cpp source/builtins/inline_func.h source/builtins/keyword_func.cpp source/builtins/keyword_func.h)
