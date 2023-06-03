//
// Created by Dogs-Cute on 5/27/2023.
//

#include "object.h"

#include <utility>

namespace pups::library {
    size_t Object::static_count = 0;

    ObjectPtr Object::end_of_line(Map *map) {
        return pending;
    }

    bool Object::is(const ObjectPtr &object) {
        return this == object.get();
    }

    bool Object::is_long_str() const noexcept {
        return false;
    }

    ObjectPtr pending = std::make_shared<Pending>();

    LongStr::LongStr(IdFilePtr idFile) : m_idFile(std::move(idFile)) {

    }

    const IdFilePtr &LongStr::get() const noexcept {
        return m_idFile;
    }

    bool LongStr::is_long_str() const noexcept {
        return true;
    }
}