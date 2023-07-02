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
    protected:
        IdSet m_attr;
        MethodMap m_methods;
        std::string m_name;
    public:
        explicit Type(Map *parent);

        ObjectPtr & find(const Id &name, Map *map) override;

        std::string type_name() const noexcept override;

        std::string &get_name() noexcept;

        IdSet &get_attr() noexcept;

        MethodMap &get_methods() noexcept;
    };

    class Instance : public Object {
    protected:
        TypePtr m_type;
        ObjectMap m_attr;
        ObjectPtr m_self_ptr;

    public:
        explicit Instance(TypePtr type);

        // MUST CALL AFTER INITIALIZING!
        void add_methods(const ObjectPtr &self);

        ObjectPtr put(ObjectPtr &object, Map *map) override;

        ObjectPtr end_of_line(Map *map) override;

        ObjectPtr &find(const Id &name, Map *map) override;

        std::string type_name() const noexcept override;
    };

    class TypeInit : public Function {
    public:
        TypeInit();
    };

    void init(Constants &constants);
}

#endif //PUPS_LIB_TESTS_TYPING_H
