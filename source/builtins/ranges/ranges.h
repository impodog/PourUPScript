//
// Created by impodog on 6/22/2023.
//

#ifndef PUPS_LIB_RANGES_H
#define PUPS_LIB_RANGES_H
#include "../types/numbers.h"

namespace pups::library::builtins::ranges {
    using namespace function;

    class Range : public HasMethods {
    public:
        pups_int begin, end, step, current;

        Range(pups_int begin, pups_int end, pups_int step);

        ObjectPtr put(ObjectPtr &object, Map *map) override;

        std::string type_name() const noexcept override;

        std::string str() const noexcept override;

        FunctionCore get_method(const Id &name) override;

        bool is_ended() const noexcept;
    };

    class RangeInit : public Function {
    public:
        RangeInit();
    };

    using RangeFuncCore = std::function<ObjectPtr(Range &, FunctionArgs &, Map *)>;
    using RangeFuncMap = IdMap<RangeFuncCore>;
    extern const RangeFuncMap range_functions;

    void init(Constants &constants);
}

#endif //PUPS_LIB_RANGES_H