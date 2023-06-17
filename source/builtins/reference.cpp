//
// Created by impodog on 6/10/2023.
//

#include "reference.h"

#include <regex>

namespace pups::library::builtins::reference {
    constexpr const char *REF_TYPE_NAME = "ref";

    Reference::Reference(ObjectPtr *object) : m_ref(object) {

    }

    ObjectPtr Reference::put(ObjectPtr &object, Map *map) {
        map->throw_error(
                std::make_shared<TypeError>("References do not allow putting. Use \".data\" to dereference it."));
        return pending;
    }

    ObjectPtr &Reference::find(const Id &name, Map *map, bool *reput_this) {
        if (name == "data")
            return *m_ref;
        return Object::find(name, map, reput_this);
    }

    std::string Reference::type_name() const noexcept {
        return REF_TYPE_NAME;
    }

    TakeReferenceOf::TakeReferenceOf() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        if (args.size() != 1)
            map->throw_error(std::make_shared<ArgumentError>("For taking reference, one only argument is required."));
        else {
            return std::make_shared<Reference>(args.front());
        }
        return pending;
    }) {}

    Id id_takeReferenceOf{"", "ref"};

    void init(Constants &constants) {
        constants.add(id_takeReferenceOf, std::make_shared<TakeReferenceOf>());
    }
}
