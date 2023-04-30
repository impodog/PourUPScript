cmake_minimum_required(VERSION 3.25)
project(PourUPScript)

set(CMAKE_CXX_STANDARD 20)

add_library(PourUPScript pups.cpp cpp_lib/math.hpp pups/macros.h cpp_lib/cpp_lib.h pups/object.hpp pups/token.hpp pups/scope.hpp pups/report.hpp pups/manage.hpp pups/keywords.hpp pups/stdtype.hpp)

project(PourUPScriptTest)

set(CMAKE_CXX_STANDARD 20)

add_executable(PourUPScriptTest test.cpp)

project(PourUPScriptInterp)

set(CMAKE_CXX_STANDARD 20)

add_executable(PourUPScriptInterp interp.cpp)
