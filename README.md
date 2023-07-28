# PUPS - PourUPScript

## Introduction

PUPS is a scripting language that uses the abstract class as the base for all imaginable objects - Object.

The idea takes after SmallTalk, where everything is an object, and calls are done by sending messages to objects.

PUPS aims for being fast, flexible on top of the restricted Cpp-Style grammar, which does not fit well with some development needs.

## How to Use

PUPS is based on a library. You can copy the files from **./source** freely.

### Running a program

We provide **CMakeLists.txt** and **test.cpp** for reference. Read the twos files for more information.

To tell if the library works, build *pups_Lib_Tests* using CMake and run the executable.

## Using Puppyscript

Puppyscript is a extension to PourUPScript and is much easier to use.

To use Puppyscript, you'll need Python(tested on 3.11, need to be at least 3.10)

A Puppyscript compiler is given under ./puppyscript, just simply run "python main.py (file) [--output, -o output_path]" to compile Puppyscript

To learn more, see ./learn under the root library (just open those files with your favortite text editor).

## For more

Please go to **./learn** for details
