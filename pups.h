//
// Created by impodog on 6/11/2023.
//

#ifndef PUPS_LIB_PUPS_H
#define PUPS_LIB_PUPS_H

#include "source/control.h"
#include "source/builtins/builtins.h"

namespace pups {
    // Link commonly used classes and functions
    using library::Id, library::Map, library::Object, library::Error, library::Constants, library::Control;
    using library::ObjectPtr, library::MapPtr, library::ControlPtr;
    using library::builtins::function::Function, library::builtins::reference::Reference;
    using library::builtins::numbers::Number, library::builtins::strings::String;
    using library::builtins::containers::Array, library::builtins::containers::Pair, library::builtins::containers::HashMap;
    using library::builtins::function::HasMethods;
    using library::pending;
    using library::builtins::init, library::module_link_name, library::PUPS_LIB_VERSION;
    using library::builtins::map_open::set_module_path;
    using library::cast;

    inline void quit() {
        library::builtins::map_open::quit();
    }
}

#endif //PUPS_LIB_PUPS_H
