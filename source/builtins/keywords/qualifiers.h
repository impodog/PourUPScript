//
// Created by impodog on 6/3/2023.
//

#ifndef PUPS_LIB_QUALIFIERS_H
#define PUPS_LIB_QUALIFIERS_H

#include "../../control.h"

namespace pups::library::builtins::qualifiers {
    using QualifierCore = std::function<void(Map *map)>;

    class Qualifier : public Object {
        QualifierCore m_core;
    protected:
        explicit Qualifier(const QualifierCore &core);
    public:
        ObjectPtr put(ObjectPtr &object, Map *map) override;
    };
}

#endif //PUPS_LIB_QUALIFIERS_H
