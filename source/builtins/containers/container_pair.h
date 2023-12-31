//
// Created by impodog on 6/17/2023.
//

#ifndef PUPS_LIB_CONTAINER_PAIR_H
#define PUPS_LIB_CONTAINER_PAIR_H

#include "container_base.h"

namespace pups::library::builtins::containers {
    class Pair : public ContainerBase {
    public:
        ObjectPtr left, right;

        Pair(ObjectPtr left, ObjectPtr right);

        [[nodiscard]] std::string type_name() const noexcept override;

        [[nodiscard]] std::string str() const noexcept override;

        ObjectPtr &source_find(const Id &name, Map *map) override;

        FunctionCore get_method(const Id &name) override;
    };

    class PairInit : public Function {
    public:
        PairInit();
    };
}

#endif //PUPS_LIB_CONTAINER_PAIR_H
