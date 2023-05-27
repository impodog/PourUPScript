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

        virtual ObjectPtr put(ObjectPtr &object, Map *map) = 0;

        virtual ObjectPtr end_of_line(Map *map);

        virtual ~Object() = default;

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
        IdFile m_idFile;
    public:
        explicit LongStr(IdFile idFile);

        ObjectPtr put(ObjectPtr &object, Map *map) override;

        [[nodiscard]] const IdFile &get() const noexcept;

        [[nodiscard]] bool is_long_str() const noexcept final;
    };

    using LongStrPtr = shared_ptr<LongStr>;
}


#endif //PUPS_LIB_TESTS_OBJECT_H
