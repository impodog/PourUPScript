//
// Created by impodog on 6/17/2023.
//

#ifndef PUPS_LIB_TESTS_CONTAINER_HASHMAP_H
#define PUPS_LIB_TESTS_CONTAINER_HASHMAP_H

#include "container_base.h"
#include "container_pair.h"
#include "container_array.h"

namespace pups::library::builtins::containers {
    class HashMap : public ContainerBase {
    public:
        std::unordered_map<ObjectPtr, ObjectPtr, ObjectHash, ObjectEqual> data;

        HashMap() = default;

        explicit HashMap(FunctionArgs &args);

        [[nodiscard]] std::string type_name() const noexcept override;

        [[nodiscard]] std::string str() const noexcept override;

        void push_args(FunctionArgs &args);

        [[nodiscard]] bool empty() const noexcept;

        ObjectPtr pop(const ObjectPtr &key);

        void clear() noexcept;

        [[nodiscard]] size_t size() const noexcept;

        FunctionCore get_method(const pups::library::Id &name) override;
    };

    class HashMapInit : public Function {
    public:
        HashMapInit();
    };

    extern const ContainerCoreMap<HashMap> hashmap_cores;
}

#endif //PUPS_LIB_TESTS_CONTAINER_HASHMAP_H
