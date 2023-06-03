//
// Created by Dogs-Cute on 5/27/2023.
//

#include "object.h"

#include <utility>

namespace pups::library {
    size_t Object::static_count = 0;

    Object::Object() : m_count(static_count++) {}

    ObjectPtr Object::end_of_line(Map *map) {
        return pending;
    }

    bool Object::is(const ObjectPtr &object) {
        return this == object.get();
    }

    bool Object::is_long_str() const noexcept {
        return false;
    }

    std::string Object::str() const noexcept {
        return "<Object:" + std::to_string(m_count) + ">";
    }

    bool Object::condition() const noexcept {
        return true;
    }

    std::string Pending::str() const noexcept {
        return "<Object:Pending>";
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