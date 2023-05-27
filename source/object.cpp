//
// Created by Dogs-Cute on 5/27/2023.
//

#include "object.h"

#include <utility>

namespace pups::library {
    size_t Object::static_count = 0;

    bool Object::is_long_str() const noexcept {
        return false;
    }

    ObjectPtr Object::end_of_line(Map *map) {
        return pending;
    }

    ObjectPtr pending = std::make_shared<Pending>();
}