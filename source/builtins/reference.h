//
// Created by impodog on 6/10/2023.
//

#ifndef PUPS_LIB_REFERENCE_H
#define PUPS_LIB_REFERENCE_H

#include "function.h"

namespace pups::library::builtins::reference {
    using namespace function;

    class Reference : public Object {
        using ObjectPtrPtr = ObjectPtr *;
        ObjectPtrPtr m_ref;
    protected:
    public:
        explicit Reference(ObjectPtr *object);

        ObjectPtr put(ObjectPtr &object, Map *map) override;

        ObjectPtr &find(const Id &name, Map *map) override;

        [[nodiscard]] std::string type_name() const noexcept override;
    };

    class TakeReferenceOf : public Function {
    public:
        TakeReferenceOf();
    };

    void init(Constants &constants);
}

#endif //PUPS_LIB_REFERENCE_H
