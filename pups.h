//
// Created by impodog on 6/11/2023.
//

#ifndef PUPS_LIB_TESTS_PUPS_H
#define PUPS_LIB_TESTS_PUPS_H

#include "source/control.h"
#include "source/builtins/builtins.h"

namespace pups {
    // Link commonly used classes and functions
    using library::Id, library::Map, library::Object, library::Error, library::Constants, library::Control;
    using library::builtins::function::Function, library::builtins::reference::Reference;
    using library::builtins::numbers::Number, library::builtins::strings::String;
    constexpr const auto &init_builtins = library::builtins::init;
}

#endif //PUPS_LIB_TESTS_PUPS_H
