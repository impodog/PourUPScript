#include "type_judge.h"

namespace pups::modules::type_judge {
    using namespace library::builtins::function;
    using namespace pups::library::builtins;

    template<typename JudgeType>
    bool judge(const ObjectPtr &object) {
        auto ptr = std::dynamic_pointer_cast<JudgeType>(object);
        return ptr != nullptr;
    }

    template<typename JudgeType>
    ObjectPtr judge_obj(const ObjectPtr &object) {
        return judge<JudgeType>(object) ? numbers::True : numbers::False;
    }

    template<typename JudgeType>
    ObjectPtr judge_obj_func(FunctionArgs &args, Map *map) {
        if (args.size() != 1)
            map->throw_error(std::make_shared<library::ArgumentError>("Type judge functions require one only argument."));
        else
            return judge_obj<JudgeType>(*args.front());
        return pending;
    }

    Id id_tpjudge{"", "tpjudge"};
    Id id_is_int{"", "is_int"}, id_is_float{"", "is_float"}, id_is_str{"", "is_str"};

    void init(Constants &constants) {
        auto &tpjudge = constants.new_sub_const(id_tpjudge);
        tpjudge.add(id_is_int, std::make_shared<Function>(judge_obj_func<numbers::IntType>));
        tpjudge.add(id_is_float, std::make_shared<Function>(judge_obj_func<numbers::FloatType>));
        tpjudge.add(id_is_str, std::make_shared<Function>(judge_obj_func<String>));
    }
}