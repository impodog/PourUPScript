//
// Created by impodog on 6/17/2023.
//

#ifndef PUPS_LIB_TESTS_CONTAINER_ARRAY_H
#define PUPS_LIB_TESTS_CONTAINER_ARRAY_H

#include "container_base.h"

namespace pups::library::builtins::containers {
    class Array : public ContainerBase {
    public:
        std::vector<ObjectPtr> data;

        Array() = default;

        explicit Array(FunctionArgs &args);

        explicit Array(std::vector<ObjectPtr> args);

        [[nodiscard]] std::string type_name() const noexcept override;

        [[nodiscard]] std::string str() const noexcept override;

        void push_args(FunctionArgs &args);

        [[nodiscard]] bool empty() const noexcept;

        ObjectPtr &back();

        ObjectPtr pop_back();

        ObjectPtr pop_at(size_t pos);

        void clear() noexcept;

        [[nodiscard]] size_t size() const noexcept;

        void insert_at(const ObjectPtr &obj, size_t pos);
    };

    class ArrayInit : public Function {
    public:
        ArrayInit();
    };
}

#endif //PUPS_LIB_TESTS_CONTAINER_ARRAY_H
