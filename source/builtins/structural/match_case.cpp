//
// Created by impodog on 7/31/2023.
//

#include "match_case.h"

namespace pups::library::builtins::match_case {
    Match::Match(ObjectPtr target, pups::library::Map *parent_map) :
            m_target(std::move(target)), Map(parent_map, true) {

    }

    MatchInit::MatchInit() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        if (args.size() != 1) {
            map->throw_error(std::make_shared<library::ArgumentError>("match requires at one only argument."));
        } else {
            auto block = cast<LongStr>(*args.back());
            if (block) {
                auto result = std::make_shared<Match>(map->get_temp(), map);
                Control control(block->ids(), result);
                control.run();
                while (args.size() != 1) {
                    *args.front() = result;
                    args.pop_front();
                }
                return result;
            } else {
                map->throw_error(std::make_shared<library::TypeError>("match requires a LongStr in the back."));
            }
        }
        return pending;
    }) {}

    bool CaseLabel::do_match_with(Match *match) {
        if (match->m_is_matched) {
            if (match->m_fall_through) {
                match->m_fall_through = false;
                return true;
            } else {
                return false;
            }
        }
        if (match->m_target->equal(match->get_temp())) {
            match->m_is_matched = true;
            return true;
        } else {
            return false;
        }
    }

    void CaseLabel::do_block(const LongStrPtr &block, Match *match) {
        auto sub_map = std::make_shared<Map>(match, true);
        Control control(block->ids(), sub_map);
        control.run();
    }

    CaseLabel::CaseLabel(bool is_default) : Function([is_default](FunctionArgs &args, Map *map) -> ObjectPtr {
        if (args.size() != 1) {
            map->throw_error(std::make_shared<library::ArgumentError>("case label requires one only argument."));
            return pending;
        }
        auto match = find_closest_match(map);
        if (!match) {
            map->throw_error(std::make_shared<library::TypeError>("case label must be in a match block."));
            return pending;
        }
        auto block = cast<LongStr>(*args.front());
        if (!block) {
            map->throw_error(std::make_shared<library::TypeError>("case label must get a LongStr in the back."));
            return pending;
        }
        if (is_default) {
            if (!match->m_is_matched || match->m_fall_through) {
                do_block(block, match);
            }
        } else {
            if (do_match_with(match)) {
                do_block(block, match);
            }
        }
        return pending;
    }) {}

    FallThrough::FallThrough() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        if (!args.empty()) {
            map->throw_error(std::make_shared<library::ArgumentError>("fall_through function requires no argument."));
        }
        auto match = find_closest_match(map);
        if (match) {
            match->m_fall_through = true;
        } else {
            map->throw_error(std::make_shared<library::TypeError>("fall_through function must be in a match block."));
        }
        return pending;
    }) {}

    Match *find_closest_match(Map *map) {
        while (map) {
            auto match = dynamic_cast<Match *>(map);
            if (match) {
                return match;
            }
            map = map->get_parent();
        }
        return nullptr;
    }

    Id id_matchInit{"", "match"}, id_caseLabel{"", "case"}, id_defaultLabel{"", "default"},
            id_fallThrough{"", "fall_through"};

    void init(Constants &constants) {
        constants.add(id_matchInit, std::make_shared<MatchInit>());
        constants.add(id_caseLabel, std::make_shared<CaseLabel>(false));
        constants.add(id_defaultLabel, std::make_shared<CaseLabel>(true));
        constants.add(id_fallThrough, std::make_shared<FallThrough>());
    }
}

