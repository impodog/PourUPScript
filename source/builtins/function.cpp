//
// Created by Dogs-Cute on 5/27/2023.
//

#include "function.h"

#include <utility>

namespace pups::library::builtins {
    Function::Function(IdFile idFile) : m_idFile(std::move(idFile)) {

    }

    ObjectPtr Function::put(ObjectPtr &object, Map *map) {
        return nullptr;
    }

    ObjectPtr Function::end_of_line(Map *map) {
        return pending;
    }

    void init_function() {

    }
}