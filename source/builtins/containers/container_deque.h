//
// Created by impodog on 7/23/2023.
//

#ifndef PUPS_LIB_CONTAINER_DEQUE_H
#define PUPS_LIB_CONTAINER_DEQUE_H

#include "container_base.h"

namespace pups::library::builtins::containers {
    class Deque : public ContainerBase {
    public:
        std::deque<ObjectPtr> data;

        Deque() = default;

        explicit Deque(FunctionArgs &args);

        explicit Deque(std::deque<ObjectPtr> args);

        [[nodiscard]] std::string type_name() const noexcept override;

        [[nodiscard]] std::string str() const noexcept override;

        void push_back_args(FunctionArgs &args);

        void push_front_args(FunctionArgs &args);

        [[nodiscard]] bool empty() const noexcept;

        ObjectPtr &back();

        ObjectPtr &front();

        ObjectPtr pop_back();

        ObjectPtr pop_front();

        void clear() noexcept;

        [[nodiscard]] size_t size() const noexcept;

        FunctionCore get_method(const Id &name) override;
    };

    class DequeInit : public Function {
    public:
        DequeInit();
    };

    extern const ContainerCoreMap<Deque> deque_cores;
}

#endif //PUPS_LIB_CONTAINER_DEQUE_H
