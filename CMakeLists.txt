cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 17)

if(NOT DEFINED ${PUPS_WORKING_DIRECTORY})
    set(PUPS_WORKING_DIRECTORY .)
endif()

set(PUPS_SOURCE ${PUPS_WORKING_DIRECTORY}/source)
set(PUPS_MODULES ${PUPS_WORKING_DIRECTORY}/modules)

set(source_files
        ${PUPS_WORKING_DIRECTORY}/pups.h
        ${PUPS_MODULES}/modules.h
        ${PUPS_SOURCE}/const.h
        ${PUPS_SOURCE}/syntax.cpp ${PUPS_SOURCE}/syntax.h
        ${PUPS_SOURCE}/object.cpp ${PUPS_SOURCE}/object.h
        ${PUPS_SOURCE}/map.cpp ${PUPS_SOURCE}/map.h
        ${PUPS_SOURCE}/control.cpp ${PUPS_SOURCE}/control.h
        ${PUPS_SOURCE}/error.cpp ${PUPS_SOURCE}/error.h
        ${PUPS_SOURCE}/builtins/types/numbers.cpp ${PUPS_SOURCE}/builtins/types/numbers.h
        ${PUPS_SOURCE}/builtins/function.cpp ${PUPS_SOURCE}/builtins/function.h
        ${PUPS_SOURCE}/builtins/builtins.h ${PUPS_SOURCE}/builtins/builtins.cpp
        ${PUPS_SOURCE}/builtins/keywords/inline_func.cpp ${PUPS_SOURCE}/builtins/keywords/inline_func.h
        ${PUPS_SOURCE}/builtins/keywords/keyword_func.cpp ${PUPS_SOURCE}/builtins/keywords/keyword_func.h
        ${PUPS_SOURCE}/constants.cpp ${PUPS_SOURCE}/constants.h
        ${PUPS_SOURCE}/builtins/types/strings.cpp ${PUPS_SOURCE}/builtins/types/strings.h
        ${PUPS_SOURCE}/builtins/keywords/qualifiers.cpp ${PUPS_SOURCE}/builtins/keywords/qualifiers.h
        ${PUPS_SOURCE}/builtins/keywords/map_open.cpp ${PUPS_SOURCE}/builtins/keywords/map_open.h
        ${PUPS_SOURCE}/builtins/keywords/builtin_func.cpp ${PUPS_SOURCE}/builtins/keywords/builtin_func.h
        ${PUPS_SOURCE}/builtins/types/reference.cpp ${PUPS_SOURCE}/builtins/types/reference.h
        ${PUPS_SOURCE}/builtins/containers/containers.cpp ${PUPS_SOURCE}/builtins/containers/containers.h
        ${PUPS_SOURCE}/builtins/containers/container_array.cpp ${PUPS_SOURCE}/builtins/containers/container_array.h
        ${PUPS_SOURCE}/builtins/containers/container_base.cpp ${PUPS_SOURCE}/builtins/containers/container_base.h
        ${PUPS_SOURCE}/builtins/containers/container_pair.cpp ${PUPS_SOURCE}/builtins/containers/container_pair.h
        ${PUPS_SOURCE}/builtins/containers/container_hashmap.cpp ${PUPS_SOURCE}/builtins/containers/container_hashmap.h
        ${PUPS_SOURCE}/builtins/ranges/ranges.cpp ${PUPS_SOURCE}/builtins/ranges/ranges.h
        ${PUPS_SOURCE}/builtins/types/typing.cpp ${PUPS_SOURCE}/builtins/types/typing.h
        ${PUPS_SOURCE}/builtins/keywords/inline_typing.cpp ${PUPS_SOURCE}/builtins/keywords/inline_typing.h
        ${PUPS_SOURCE}/builtins/containers/container_deque.cpp ${PUPS_SOURCE}/builtins/containers/container_deque.h
        ${PUPS_SOURCE}/builtins/debugger/debug_info.cpp ${PUPS_SOURCE}/builtins/debugger/debug_info.h)

set(std_files ${PUPS_MODULES}/std/std.h ${PUPS_MODULES}/std/std.cpp
        ${PUPS_MODULES}/std/random.cpp ${PUPS_MODULES}/std/random.h
        ${PUPS_MODULES}/std/string_code.cpp ${PUPS_MODULES}/std/string_code.h
        ${PUPS_MODULES}/std/type_judge.cpp ${PUPS_MODULES}/std/type_judge.h
        ${PUPS_MODULES}/std/memory.cpp ${PUPS_MODULES}/std/memory.h
        ${PUPS_MODULES}/std/sys.cpp ${PUPS_MODULES}/std/sys.h
        ${PUPS_MODULES}/std/ios.cpp ${PUPS_MODULES}/std/ios.h
        ${PUPS_MODULES}/std/sutil.cpp ${PUPS_MODULES}/std/sutil.h
        ${PUPS_MODULES}/std/pupsmath.cpp ${PUPS_MODULES}/std/pupsmath.h)

project(pups_Lib)

add_library(pups_Lib ${source_files})

project(pups_Lib_Tests)

add_executable(pups_Lib_Tests test.cpp ${source_files} ${std_files})
