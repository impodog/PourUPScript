//
// Created by Dogs-Cute on 5/27/2023.
//

#ifndef PUPS_LIB_TESTS_OBJECT_H
#define PUPS_LIB_TESTS_OBJECT_H

#include "syntax.h"

namespace pups::library {
    class Object;

    using ObjectPtr = shared_ptr<Object>;

    class Map;

    using MapPtr = shared_ptr<Map>;

    using Arguments = queue<ObjectPtr &>;

    class Object {
    protected:
        static size_t static_count;
    public:
        size_t m_count = static_count++;

        // Put the next given object into the base. The return value is for switching base.
        // Most of the time, you would want to return nullptr for not switching base.
        virtual ObjectPtr put(ObjectPtr &object, Map *map) = 0;

        // Mark an end of this line. The return value is stored in Map::m_temp.
        virtual ObjectPtr end_of_line(Map *map);

        virtual ~Object() = default;

        bool is(const ObjectPtr &object);

        [[nodiscard]] virtual bool is_long_str() const noexcept;
    };

    class Pending final : public Object {
    protected:
        ObjectPtr m_object;
    public:
        ObjectPtr put(ObjectPtr &object, Map *map) override;
    };

    extern ObjectPtr pending;

    class LongStr : public Object {
    protected:
        IdFilePtr m_idFile;
    public:
        explicit LongStr(IdFilePtr idFile);

        ObjectPtr put(ObjectPtr &object, Map *map) override;

        [[nodiscard]] const IdFilePtr &get() const noexcept;

        [[nodiscard]] bool is_long_str() const noexcept final;
    };

    class Symbol final : public Object {
    public:
        Symbol() = default;

        ObjectPtr put(ObjectPtr &object, Map *map) override;
    };

    using LongStrPtr = shared_ptr<LongStr>;
}


#endif //PUPS_LIB_TESTS_OBJECT_H
