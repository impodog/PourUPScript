//
// Created by impodog on 7/31/2023.
//

#ifndef PUPS_LIB_MATCH_CASE_H
#define PUPS_LIB_MATCH_CASE_H

#include "enumerate.h"

namespace pups::library::builtins::match_case {
    using namespace function;

    class Match : public Map {
        ObjectPtr m_target;
        bool m_is_matched = false, m_fall_through = false;

        friend class CaseLabel;

        friend class FallThrough;

    public:
        Match(ObjectPtr target, Map *parent_map);
    };

    class MatchInit : public Function {
    public:
        MatchInit();
    };

    class CaseLabel : public Function {
        static bool do_match_with(Match *match);

        static void do_block(const LongStrPtr &block, Match *match);

    public:
        explicit CaseLabel(bool is_default);
    };

    class FallThrough : public Function {
    public:
        FallThrough();
    };

    Match *find_closest_match(Map *map);

    void init(Constants &constants);
}

#endif //PUPS_LIB_MATCH_CASE_H
