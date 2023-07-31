cmake_minimum_required(VERSION 3.26)
project(cpp_puppy)

set(CMAKE_CXX_STANDARD 17)

add_executable(cpp_puppy main.cpp
        const.h
        processor.h
        processor.cpp
        preprocess.cpp
        extract.cpp
        mid_process.cpp
        macro_rules.cpp
        syntactic.cpp
        type_format.cpp
        structure.cpp
        assignment.cpp
        brackets.cpp
        expr_optim.cpp
        shortcuts.cpp
)