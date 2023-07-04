//
// Created by impodog on 7/1/2023.
//

#ifndef PUPS_LIB_TESTS_TYPING_H
#define PUPS_LIB_TESTS_TYPING_H

#include "../function.h"

namespace pups::library::builtins::typing {
    using namespace function;

    class Type;

    using TypePtr = std::shared_ptr<Type>;
    using MethodMap = IdMap<ObjectPtr>;

    class Instance;

    using InstancePtr = std::shared_ptr<Instance>;

    class Type : public Map {
        struct hash {
            size_t operator()(const TypePtr &type) const noexcept;
        };

    protected:
        using ParentSet = std::unordered_set<TypePtr>;
        ParentSet m_parents;
        IdSet m_attr;
        MethodMap m_methods;
        std::string m_name;
    public:
        explicit Type(Map *parent);

        ObjectPtr &find(const Id &name, Map *map) override;

        std::string type_name() const noexcept override;

        std::string &get_name() noexcept;

        IdSet &get_attr() noexcept;

        MethodMap &get_methods() noexcept;

        void inherit_from(const TypePtr &type) noexcept;

        bool is_subtype(const TypePtr &type) const noexcept;
    };

    class Instance : public Object {
    protected:
        TypePtr m_type;
        ObjectMap m_attr;
        ObjectPtr m_self_ptr;

    public:
        explicit Instance(TypePtr type);

        // MUST CALL AFTER INITIALIZING!
        void add_methods(const ObjectPtr &self, FunctionArgs &args, Map *map);

        ObjectPtr put(ObjectPtr &object, Map *map) override;

        ObjectPtr end_of_line(Map *map) override;

        ObjectPtr &find(const Id &name, Map *map) override;

        std::string type_name() const noexcept override;

        TypePtr get_type() const noexcept;
    };

    class TypeInit : public Function {
    public:
        TypeInit();
    };

    void init(Constants &constants);
}

#endif //PUPS_LIB_TESTS_TYPING_H
