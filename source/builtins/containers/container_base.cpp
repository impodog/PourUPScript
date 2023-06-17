//
// Created by impodog on 6/17/2023.
//

#include "container_base.h"

namespace pups::library::builtins::containers {
    ObjectPtr ContainerBase::put(ObjectPtr &object, Map *map) {
        map->throw_error(
                std::make_shared<ArgumentError>("Container \"" + type_name() + "\" does not support putting."));
        return pending;
    }
}