cmake_minimum_required(VERSION 3.25)
project(pups_Lib)

set(CMAKE_CXX_STANDARD 17)

set(source_files source/const.h
        source/syntax.cpp source/syntax.h
        source/object.cpp source/object.h
        source/map.cpp source/map.h
        source/control.cpp source/control.h
        source/error.cpp source/error.h
        source/builtins/numbers.cpp source/builtins/numbers.h
        source/builtins/function.cpp source/builtins/function.h
        source/builtins/builtins.h source/builtins/builtins.cpp
        source/builtins/inline_func.cpp source/builtins/inline_func.h
        source/builtins/keyword_func.cpp source/builtins/keyword_func.h
        source/constants.cpp source/constants.h
        source/builtins/strings.cpp source/builtins/strings.h
        source/builtins/qualifiers.cpp source/builtins/qualifiers.h
        source/builtins/map_open.cpp source/builtins/map_open.h
        source/builtins/builtin_func.cpp source/builtins/builtin_func.h
        source/builtins/reference.cpp source/builtins/reference.h
        )

add_library(pups_Lib ${source_files})

project(pups_Lib_Tests)

add_executable(pups_Lib_Tests test.cpp ${source_files})
