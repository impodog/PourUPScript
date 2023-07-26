//
// Created by Dogs-Cute on 5/27/2023.
//

#include "object.h"

#include <utility>

namespace pups::library {
    size_t Object::static_count = 0;

    FindError::FindError(const Object *sender, Id id) : sender(sender), id(std::move(id)) {}

    Object::Object() : m_count(static_count++) {

    }

    bool Object::is(const ObjectPtr &object) {
        return this == object.get();
    }

    bool Object::is_long_str() const noexcept {
        return false;
    }

    std::string Object::str() const noexcept {
        return "<" + type_name() + ":" + std::to_string(m_count) + " at " + to_string(this) + ">";
    }

    std::string Object::repr() const noexcept {
        return str();
    }

    bool Object::condition() const noexcept {
        return true;
    }

    std::string Object::type_name() const noexcept {
        return "object";
    }

    size_t Object::hash() noexcept {
        return std::hash<const void *>()(reinterpret_cast<const void *>(this));
    }

    bool Object::equal(ObjectPtr &object) noexcept {
        return this == object.get();
    }

    size_t ObjectHash::operator()(const ObjectPtr &object) const noexcept {
        return object->hash();
    }

    bool ObjectEqual::operator()(const ObjectPtr &lhs, ObjectPtr rhs) const noexcept {
        return lhs->equal(rhs);
    }

    std::string Pending::type_name() const noexcept {
        return "pending";
    }

    bool Pending::condition() const noexcept {
        return false;
    }

    ObjectPtr pending = std::make_shared<Pending>();

    LongStr::LongStr(IdFilePtr idFile) : m_idFile(std::move(idFile)) {

    }

    const IdFilePtr &LongStr::ids() const noexcept {
        return m_idFile;
    }

    bool LongStr::is_long_str() const noexcept {
        return true;
    }

    Symbol::Symbol(std::string name) : m_name(std::move(name)) {}

    Id template_name(std::string name, const std::vector<std::string> &templates) {
        name = "_T_" + name;
        for (const auto &obj: templates)
            name.append("_" + obj);
        return Id{"", name};
    }

    Id module_link_name(const std::string &name) {
        return Id{"", MODULE_LINK + name};
    }

    bool is_pending(const ObjectPtr &object) noexcept {
        return object.get() == pending.get();
    }
}