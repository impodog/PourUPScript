cmake_minimum_required(VERSION 3.27)

set(CXX_STANDARD 17)

project(pups_std)

set(PUPS_PATH "../../..")

include_directories(${PUPS_PATH})

link_directories(${PUPS_PATH}/cmake-build-release)

add_library(pups_std SHARED
        ios.cpp
        ios.h
        memory.cpp
        memory.h
        pupsmath.cpp
        pupsmath.h
        random.cpp
        random.h
        std.cpp
        std.h
        string_code.cpp
        string_code.h
        sutil.cpp
        sutil.h
        sys.cpp
        sys.h
        type_judge.cpp
        type_judge.h
)

target_link_libraries(pups_std pups_Lib)