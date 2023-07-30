//
// Created by impodog on 7/30/2023.
//

#include "compare.h"

namespace pups::library::builtins::compare {
    Lower::Lower(Map *map) : map(map) {}

    bool Lower::operator()(const ObjectPtr &lhs, ObjectPtr rhs) const {
        return lhs->compare(rhs, map) == -1;
    }

    LowerEqual::LowerEqual(Map *map) : map(map) {}

    bool LowerEqual::operator()(const ObjectPtr &lhs, ObjectPtr rhs) const {
        auto result = lhs->compare(rhs, map);
        return result == -1 || result == 0;
    }

    Greater::Greater(Map *map) : map(map) {}

    bool Greater::operator()(const ObjectPtr &lhs, ObjectPtr rhs) const {
        return lhs->compare(rhs, map) == 1;
    }

    GreaterEqual::GreaterEqual(Map *map) : map(map) {}

    bool GreaterEqual::operator()(const ObjectPtr &lhs, ObjectPtr rhs) const {
        auto result = lhs->compare(rhs, map);
        return result == 1 || result == 0;
    }

    Equal::Equal(Map *map) : map(map) {}

    bool Equal::operator()(const ObjectPtr &lhs, ObjectPtr rhs) const {
        return lhs->compare(rhs, map) == 0;
    }

    NotEqual::NotEqual(Map *map) : map(map) {}

    bool NotEqual::operator()(const ObjectPtr &lhs, ObjectPtr rhs) const {
        return lhs->compare(rhs, map) != 0;
    }

    template<typename Compare>
    class CompareFunction : public Function {
    public:
        CompareFunction() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
            if (args.size() != 2) {
                map->throw_error(std::make_shared<ArgumentError>("compare function should receive 2 only arguments."
                                                                 " pending is returned."));
            } else {
                return std::make_shared<numbers::BoolType>(Compare(map)(*args.front(), *args.back()));
            }
            return pending;
        }) {}
    };

    CompareFunc get_compare(const std::shared_ptr<Function> &func, Map *map) {
        return [func, map](ObjectPtr lhs, ObjectPtr rhs) -> bool {
            func->put(lhs, map);
            func->put(rhs, map);
            return cast<numbers::BoolType>(func->end_of_line(map))->value;
        };
    }

    Id id_lower{"", "lt"}, id_lowerEqual{"", "le"},
            id_greater{"", "gt"}, id_greaterEqual{"", "ge"},
            id_equal{"", "eq"}, id_notEqual{"", "ne"};

    void init(Constants &constants) {
        constants.add(id_lower, std::make_shared<CompareFunction<Lower>>());
        constants.add(id_lowerEqual, std::make_shared<CompareFunction<LowerEqual>>());
        constants.add(id_greater, std::make_shared<CompareFunction<Greater>>());
        constants.add(id_greaterEqual, std::make_shared<CompareFunction<GreaterEqual>>());
        constants.add(id_equal, std::make_shared<CompareFunction<Equal>>());
        constants.add(id_notEqual, std::make_shared<CompareFunction<NotEqual>>());
    }
}
