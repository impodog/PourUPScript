//
// Created by impodog on 6/17/2023.
//

#ifndef PUPS_LIB_CONTAINER_BASE_H
#define PUPS_LIB_CONTAINER_BASE_H

#include "../function.h"
#include "../types/reference.h"
#include "../types/numbers.h"

namespace pups::library::builtins::containers {
    using namespace function;
    constexpr cstr name_array = "array", name_pair = "pair", name_hashmap = "hashmap";

    class ContainerBase : public HasMethods {
    public:
        ObjectPtr put(ObjectPtr &object, Map *map) override;
    };

    using ContainerPtr = std::shared_ptr<ContainerBase>;
    template<typename Container>
    using ContainerOperatorCore = std::function<ObjectPtr(Container &, FunctionArgs &, Map *)>;
    template<typename Container>
    using ContainerCoreMap = IdMap<ContainerOperatorCore<Container>>;
}

#endif //PUPS_LIB_CONTAINER_BASE_H
