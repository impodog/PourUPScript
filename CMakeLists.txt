cmake_minimum_required(VERSION 3.25)
project(pups_Lib)

set(CMAKE_CXX_STANDARD 17)

set(source_files source/const.h
        source/syntax.cpp source/syntax.h
        source/object.cpp source/object.h
        source/map.cpp source/map.h
        source/control.cpp source/control.h
        source/error.cpp source/error.h
        source/builtins/types/numbers.cpp source/builtins/types/numbers.h
        source/builtins/function.cpp source/builtins/function.h
        source/builtins/builtins.h source/builtins/builtins.cpp
        source/builtins/keywords/inline_func.cpp source/builtins/keywords/inline_func.h
        source/builtins/keywords/keyword_func.cpp source/builtins/keywords/keyword_func.h
        source/constants.cpp source/constants.h
        source/builtins/types/strings.cpp source/builtins/types/strings.h
        source/builtins/keywords/qualifiers.cpp source/builtins/keywords/qualifiers.h
        source/builtins/keywords/map_open.cpp source/builtins/keywords/map_open.h
        source/builtins/keywords/builtin_func.cpp source/builtins/keywords/builtin_func.h
        source/builtins/types/reference.cpp source/builtins/types/reference.h
        pups.h source/builtins/containers/containers.cpp source/builtins/containers/containers.h
        source/builtins/containers/container_array.cpp source/builtins/containers/container_array.h
        source/builtins/containers/container_base.cpp source/builtins/containers/container_base.h
        source/builtins/containers/container_pair.cpp source/builtins/containers/container_pair.h
        source/builtins/containers/container_hashmap.cpp source/builtins/containers/container_hashmap.h
        source/builtins/ranges/ranges.cpp source/builtins/ranges/ranges.h)

set(std_files modules/std/std.h modules/std/std.cpp
        modules/std/random.cpp modules/std/random.h
        modules/std/convert.cpp modules/std/convert.h
        modules/std/string_code.cpp modules/std/string_code.h
        modules/std/type_judge.cpp modules/std/type_judge.h
        modules/std/memory.cpp modules/std/memory.h
        modules/std/sys.cpp modules/std/sys.h)

add_library(pups_Lib ${source_files})

project(pups_Lib_Tests)

add_executable(pups_Lib_Tests test.cpp ${source_files} ${std_files})
