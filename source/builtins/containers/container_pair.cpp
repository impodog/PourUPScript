//
// Created by impodog on 6/17/2023.
//

#include "container_pair.h"

namespace pups::library::builtins::containers {
    Pair::Pair(ObjectPtr left, ObjectPtr right) : left(std::move(left)), right(std::move(right)) {}

    std::string Pair::type_name() const noexcept {
        return name_pair;
    }

    std::string Pair::str() const noexcept {
        return type_name() + "{" + left->str() + ", " + right->str() + "}";
    }

    ObjectPtr &Pair::find(const Id &name, Map *map, bool *reput_this) {
        if (name == "left")
            return left;
        if (name == "right")
            return right;
        return ContainerBase::find(name, map, reput_this);
    }

    PairInit::PairInit() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        if (args.size() != 2)
            map->throw_error(std::make_shared<ArgumentError>("Pair initialization requires two only arguments"));
        else {
            auto &left = *args.front();
            args.pop();
            auto &right = *args.front();
            args.pop();
            return std::make_shared<Pair>(left, right);
        }
        return pending;
    }) {}
}