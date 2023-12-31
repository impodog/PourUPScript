//
// Created by Dogs-Cute on 5/27/2023.
//

#ifndef PUPS_LIB_OBJECT_H
#define PUPS_LIB_OBJECT_H

#include "syntax.h"
#include "debug/record.h"

namespace pups::library {
    class Object;

    using ObjectPtr = std::shared_ptr<Object>;

    class Map;

    using MapPtr = std::shared_ptr<Map>;

    using Arguments = std::queue<ObjectPtr &>;

    class FindError : public std::exception {
    public:
        const Object *sender;
        Id id;

        FindError(const Object *sender, Id id);
    };

    class Object {
    protected:
        static size_t static_count;

    public:
        using ObjectMap = IdMap<ObjectPtr>;
        size_t m_count;
        static const Id id_compare;
        static constexpr int compare_failure = std::numeric_limits<int>::max();

        Object();

        // Put the next given object into the base. The return value is for switching base.
        // Most of the time, you would want to return nullptr for not switching base.
        virtual ObjectPtr put(ObjectPtr &object, Map *map) = 0;

        // Get an object from this, but this may throw an error, and is overridable
        virtual ObjectPtr &source_find(const Id &name, Map *map);

        // Get an object from this as in "[this].name"
        ObjectPtr &find(const Id &name, Map *map);

        // Mark an end of this line. The return value is stored in Map::m_temp.
        virtual ObjectPtr end_of_line(Map *map);

        virtual ~Object() = default;

        bool is(const ObjectPtr &object);

        [[nodiscard]] virtual bool is_long_str() const noexcept;

        [[nodiscard]] virtual std::string str() const noexcept;

        [[nodiscard]] virtual std::string repr() const noexcept;

        [[nodiscard]] virtual bool condition() const noexcept;

        [[nodiscard]] virtual std::string type_name() const noexcept;

        [[nodiscard]] virtual size_t hash() noexcept;

        virtual bool equal(ObjectPtr &object) noexcept;

        virtual int compare(ObjectPtr &object, Map *map) noexcept;
    };

    using ObjectMap = Object::ObjectMap;

    struct ObjectHash {
        size_t operator()(const ObjectPtr &object) const noexcept;
    };

    struct ObjectEqual {
        bool operator()(const ObjectPtr &lhs, ObjectPtr rhs) const noexcept;
    };

    class Pending final : public Object {
    public:
        ObjectPtr put(ObjectPtr &object, Map *map) override;

        [[nodiscard]] std::string type_name() const noexcept override;

        [[nodiscard]] bool condition() const noexcept override;
    };

    extern ObjectPtr pending;

    class LongStr : public Object {
    protected:
        IdFile m_idFile;
    public:
        explicit LongStr(IdFile idFile);

        ObjectPtr put(ObjectPtr &object, Map *map) override;

        [[nodiscard]] const IdFile &ids() const noexcept;

        [[nodiscard]] bool is_long_str() const noexcept final;
    };

    class Symbol final : public Object {
        std::string m_name;
    public:
        explicit Symbol(std::string name);

        ObjectPtr put(ObjectPtr &object, Map *map) override;
    };

    using LongStrPtr = std::shared_ptr<LongStr>;

    template<typename ObjectType>
    std::shared_ptr<ObjectType> cast(const ObjectPtr &object) {
        static_assert(std::is_base_of<Object, ObjectType>::value, "Object casting requires a object type.");
        return std::dynamic_pointer_cast<ObjectType>(object);
    }

    Id template_name(std::string name, const std::vector<std::string> &templates);

    Id module_link_name(const std::string &name);

    bool is_pending(const ObjectPtr &object) noexcept;
}


#endif //PUPS_LIB_OBJECT_H
