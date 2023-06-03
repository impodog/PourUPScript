//
// Created by impodog on 6/3/2023.
//

#include "strings.h"

#include <utility>

namespace pups::library::builtins::strings {
    String::String(std::string data) : m_data(std::move(data)) {

    }

    ObjectPtr String::put(ObjectPtr &object, Map *map) {
        auto ptr = std::dynamic_pointer_cast<String>(object);
        if (ptr) {
            return std::make_shared<String>(m_data + ptr->m_data);
        } else {
            map->throw_error(std::make_shared<TypeError>("Cannot put anything other than string into string."));
            return pending;
        }
    }

    std::string String::str() const noexcept {
        return m_data;
    }

    const std::string &String::data() const noexcept {
        return m_data;
    }


    void init(Constants &constants) {

    }
}
