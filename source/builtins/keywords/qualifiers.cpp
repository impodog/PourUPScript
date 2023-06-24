//
// Created by impodog on 6/3/2023.
//

#include "qualifiers.h"

namespace pups::library::builtins::qualifiers {
    Qualifier::Qualifier(const QualifierCore &core) : m_core(core) {

    }

    ObjectPtr Qualifier::put(ObjectPtr &object, Map *map) {
        m_core(map);
        return object;
    }
}