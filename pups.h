//
// Created by impodog on 6/11/2023.
//

#ifndef PUPS_LIB_PUPS_H
#define PUPS_LIB_PUPS_H

#include "source/control.h"
#include "source/builtins/builtins.h"
#include "source/package/package.h"

namespace pups {
    // Link commonly used classes and functions
    using library::Id, library::Map, library::Object, library::Error, library::Constants, library::Control;
    using library::ObjectPtr, library::MapPtr, library::ControlPtr;
    using library::builtins::function::Function, library::builtins::reference::Reference;
    using library::builtins::numbers::Number, library::builtins::strings::String;
    using library::builtins::containers::Array, library::builtins::containers::Pair, library::builtins::containers::HashMap;
    using library::builtins::function::HasMethods;
    using library::builtins::function::FunctionArgs, library::builtins::function::FunctionCore;
    using library::package::PackageHandler;
    using library::pending, library::builtins::function::empty_func;
    using library::builtins::numbers::True, library::builtins::numbers::False;
    using library::module_link_name, library::PUPS_LIB_VERSION;
    using library::builtins::map_open::set_module_path;
    using library::cast;

    inline void init(Constants &constants) {
        library::builtins::init(constants);
        library::package::init(constants);
    }

    inline void quit() {
        library::builtins::map_open::quit();
        library::package::quit();
    }
}

#endif //PUPS_LIB_PUPS_H
