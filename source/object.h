//
// Created by Dogs-Cute on 5/27/2023.
//

#ifndef PUPS_LIB_TESTS_OBJECT_H
#define PUPS_LIB_TESTS_OBJECT_H

#include "syntax.h"
#include "debug/record.h"

namespace pups::library {
    class Object;

    using ObjectPtr = std::shared_ptr<Object>;

    class Map;

    using MapPtr = std::shared_ptr<Map>;

    using Arguments = std::queue<ObjectPtr &>;

    class Object {
    protected:
        static size_t static_count;
    public:
        using ObjectMap = IdMap<ObjectPtr>;
        size_t m_count;

        Object();

        // Put the next given object into the base. The return value is for switching base.
        // Most of the time, you would want to return nullptr for not switching base.
        virtual ObjectPtr put(ObjectPtr &object, Map *map) = 0;

        // Get an object from this as in "[this].name"
        virtual ObjectPtr &find(const Id &name, Map *map);

        // Mark an end of this line. The return value is stored in Map::m_temp.
        virtual ObjectPtr end_of_line(Map *map);

        virtual ~Object() = default;

        bool is(const ObjectPtr &object);

        [[nodiscard]] virtual bool is_long_str() const noexcept;

        [[nodiscard]] virtual std::string str() const noexcept;

        [[nodiscard]] virtual bool condition() const noexcept;

        [[nodiscard]] virtual std::string type_name() const noexcept;

        [[nodiscard]] virtual size_t hash() const noexcept;

        virtual size_t equal(const ObjectPtr &object) const noexcept;
    };

    using ObjectMap = Object::ObjectMap;

    struct ObjectHash {
        size_t operator()(const ObjectPtr &object) const;
    };

    struct ObjectEqual {
        size_t operator()(const ObjectPtr &lhs, const ObjectPtr &rhs) const;
    };

    class Pending final : public Object {
    public:
        ObjectPtr put(ObjectPtr &object, Map *map) override;

        [[nodiscard]] std::string str() const noexcept override;

        [[nodiscard]] bool condition() const noexcept override;
    };

    extern ObjectPtr pending;

    class LongStr : public Object {
    protected:
        IdFilePtr m_idFile;
    public:
        explicit LongStr(IdFilePtr idFile);

        ObjectPtr put(ObjectPtr &object, Map *map) override;

        [[nodiscard]] const IdFilePtr &ids() const noexcept;

        [[nodiscard]] bool is_long_str() const noexcept final;
    };

    class Symbol final : public Object {
        std::string m_name;
    public:
        explicit Symbol(std::string name);

        ObjectPtr put(ObjectPtr &object, Map *map) override;
    };

    using LongStrPtr = std::shared_ptr<LongStr>;

    Id template_name(std::string name, const std::vector<std::string> &templates);
}


#endif //PUPS_LIB_TESTS_OBJECT_H
