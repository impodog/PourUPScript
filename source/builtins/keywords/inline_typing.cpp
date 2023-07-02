//
// Created by impodog on 7/1/2023.
//

#include "inline_typing.h"
#include "../types/strings.h"

namespace pups::library::builtins::inline_typing {
    TypeInit::TypeInit() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        if (args.empty())
            map->throw_error(std::make_shared<ArgumentError>("Type initialization requires at least one argument."));
        else {
            auto ids = cast<LongStr>(*args.back());
            if (ids) {
                auto new_type = std::make_shared<Type>(map);
                {
                    Control control(*ids->ids(), new_type);
                    control.run();
                }
                while (!args.empty()) {
                    *args.front() = new_type;
                    args.pop_front();
                }
                return new_type;
            } else
                map->throw_error(
                        std::make_shared<TypeError>("Type initialization requires a long str as the final argument."));
        }
        return pending;
    }) {}

    InstanceInit::InstanceInit() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        if (args.size() != 1)
            map->throw_error(std::make_shared<ArgumentError>("Type initialization requires one only argument."));
        else {
            auto type = cast<Type>(*args.front());
            if (type) {
                auto instance = std::make_shared<Instance>(type);
                instance->add_methods(instance);
                return instance;
            } else
                map->throw_error(std::make_shared<TypeError>("Instance initialization requires a type argument."));
        }
        return pending;
    }) {}

    ObjectPtr type_set_name(FunctionArgs &args, Map *map) {
        auto ptr = dynamic_cast<Type *>(map);
        if (!ptr)
            map->throw_error(
                    std::make_shared<TypeError>("Type name setting must be done within a type, not a regular map."));
        else if (args.size() != 1)
            map->throw_error(std::make_shared<ArgumentError>("Type name setting requires one only argument."));
        else {
            auto name = cast<strings::String>(*args.front());
            if (name)
                ptr->get_name() = name->data();
            else
                map->throw_error(std::make_shared<TypeError>("Type name setting requires a string argument."));
        }
        return pending;
    }

    ObjectPtr type_attr(FunctionArgs &args, Map *map) {
        auto ptr = dynamic_cast<Type *>(map);
        if (!ptr)
            map->throw_error(
                    std::make_shared<TypeError>(
                            "Type attribute setting must be done within a type, not a regular map."));
        else {
            while (!args.empty()) {
                auto name = cast<strings::String>(*args.front());
                if (name)
                    ptr->get_attr().insert(Id{name->data()});
                else
                    map->throw_error(std::make_shared<TypeError>("Type attribute setting requires string arguments."
                                                                 "Argument " + args.front()->get()->repr() +
                                                                 " skipped."));
                args.pop_front();
            }
        }
        return pending;
    }

    ObjectPtr type_method(FunctionArgs &args, Map *map) {
        auto ptr = dynamic_cast<Type *>(map);
        if (!ptr)
            map->throw_error(
                    std::make_shared<TypeError>(
                            "Type method setting must be done within a type, not a regular map."));
        else {
            while (!args.empty()) {
                auto name = cast<strings::String>(*args.front());
                if (name)
                    ptr->get_methods().insert({Id{name->data()}, pending});
                else
                    map->throw_error(std::make_shared<TypeError>("Type method setting requires string arguments."
                                                                 "Argument " + args.front()->get()->repr() +
                                                                 " skipped."));
                args.pop_front();
            }
        }
        return pending;
    }

    Id id_typeInit{"", "type"}, id_instanceInit{"", "new"};
    Id id_type_set_name{"", "tid"}, id_type_attr{"", "attr"}, id_type_method{"", "mth"};

    void init(Constants &constants) {
        constants.add(id_typeInit, std::make_shared<TypeInit>());
        constants.add(id_instanceInit, std::make_shared<InstanceInit>());
        constants.add(id_type_set_name, std::make_shared<Function>(type_set_name));
        constants.add(id_type_attr, std::make_shared<Function>(type_attr));
        constants.add(id_type_method, std::make_shared<Function>(type_method));
    }
}