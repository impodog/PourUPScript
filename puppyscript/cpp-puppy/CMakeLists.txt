cmake_minimum_required(VERSION 3.26)

set(CMAKE_CXX_STANDARD 17)

set(source
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
        export.h)

project(cpp_puppy)
add_executable(cpp_puppy main.cpp ${source})

project(cpp_puppy_library)
add_library(cpp_puppy_library SHARED ${source})
