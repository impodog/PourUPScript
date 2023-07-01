//
// Created by impodog on 6/22/2023.
//

#include "ranges.h"

namespace pups::library::builtins::ranges {
    constexpr cstr name_range = "range";

    Range::Range(pups_int begin, pups_int end, pups_int step):begin(begin), end(end), step(step), current(begin) {

    }

    ObjectPtr Range::put(ObjectPtr &object, Map *map) {
        map->throw_error(std::make_shared<ArgumentError>("Range object should not receive any objects."));
        return nullptr;
    }

    std::string Range::type_name() const noexcept {
        return name_range;
    }

    std::string Range::str() const noexcept {
        return "range{" + std::to_string(begin) + ", " + std::to_string(end) + ", " + std::to_string(step) + "}";
    }

    FunctionCore Range::get_method(const Id &name) {
        const auto &func = range_functions.at(name);
        return [this, &func](FunctionArgs &args, Map *map) -> ObjectPtr {
            return func(*this, args, map);
        };
    }

    bool Range::is_ended() const noexcept {
        return step > 0 ? current < end : current > end;
    }

    const RangeFuncMap range_functions = {
        {Id{"", "beg"}, [](Range &range, FunctionArgs &args, Map *map) -> ObjectPtr {
            range.current = range.begin;
            return std::make_shared<numbers::NumType<pups_int>>(range.current);
        }},
        {Id{"", "next"}, [](Range &range, FunctionArgs &args, Map *map) -> ObjectPtr {
            range.current += range.step;
            return std::make_shared<numbers::NumType<pups_int>>(range.current);
        }},
        {Id{"", "cond"}, [](Range &range, FunctionArgs &args, Map *map) -> ObjectPtr {
            return std::make_shared<numbers::NumType<pups_bool>>(range.is_ended());
        }},
        {Id{"", "has"}, [](Range &range, FunctionArgs &args, Map *map) -> ObjectPtr {
            if (args.size() != 1)
                map->throw_error(std::make_shared<ArgumentError>("range.has requires one only argument"));
            else {
                auto ptr = cast<numbers::IntType>(*args.front());
                if (ptr)
                    return ((range.step>0 ? 
                        (range.begin<=ptr->value && ptr->value<range.end)
                        : (range.begin>=ptr->value && ptr->value>range.end)) &&
                         (ptr->value-range.begin) % range.step == 0)
                         ? numbers::True : numbers::False;
                else
                    map->throw_error(std::make_shared<TypeError>("range.has requires an integer"));
            }
            return pending;
        }}
    };

    RangeInit::RangeInit() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        const auto next_ptr = [&args]() {
            auto ptr = cast<numbers::IntType>(*args.front());
            args.pop_front();
            if (ptr)
                return ptr;
            else
                throw std::invalid_argument("Cannot cast to numbers::IntType");
        };
        std::shared_ptr<numbers::IntType> begin_ptr, end_ptr, step_ptr;
        try {
            switch (args.size()) {
                case 1:
                    end_ptr = next_ptr();
                    break;
                case 2:
                    begin_ptr = next_ptr();
                    end_ptr = next_ptr();
                    break;
                case 3:
                    begin_ptr = next_ptr();
                    end_ptr = next_ptr();
                    step_ptr = next_ptr();
                    break;
                default:
                    map->throw_error(std::make_shared<ArgumentError>("RangeInit must receive 1, 2, or 3 arguments."));
                    return pending;
            }
        } catch (const std::invalid_argument &) {
            map->throw_error(std::make_shared<TypeError>("RangeInit must receive int arguments."));
            return pending;
        }
        pups_int begin = begin_ptr ? begin_ptr->value : 0,
            end = end_ptr->value,
            step = step_ptr ? step_ptr->value : sign_sub(end, begin);
        return std::make_shared<Range>(begin, end, step);
    }) {}

    Id id_rangeInit{"", name_range};

    void init(Constants &constants) {
        constants.add(id_rangeInit, std::make_shared<RangeInit>());
    }
}
