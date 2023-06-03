//
// Created by Dogs-Cute on 5/28/2023.
//

#include "inline_func.h"

namespace pups::library::builtins::inline_func {
    Id id_arg_catch{"", "param"};

    Arguments::Arguments(FunctionArgs args) : args(std::move(args)) {

    }

    ObjectPtr Arguments::put(ObjectPtr &object, Map *map) {
        if (args.empty())
            map->throw_error(std::make_shared<ArgumentError>("Not enough arguments given."));
        else {
            object = *args.front();
            args.pop();
        }
        return nullptr;
    }

    void InlineFunc::add_arguments(MapPtr &map, const FunctionArgs &args) {
        map->add_object(id_arg_catch, std::make_shared<Arguments>(args));
    }

    InlineFunc::InlineFunc(IdFile idFile) :
            m_idFile(std::move(idFile)),
            Function([this](const FunctionArgs &args, Map *map) -> ObjectPtr {
                MapPtr sub_map = std::make_shared<Map>(map);
                Control control(m_idFile, sub_map);
                add_arguments(sub_map, args);
                control.run();
                auto ret = sub_map->get_return();
                return ret ? ret : pending;
            }) {

    }

    InlineFunc_Init::InlineFunc_Init() :
            Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
                if (args.size() != 1 || !args.front()->get()->is_long_str()) {
                    map->throw_error(std::make_shared<ArgumentError>(
                            "Inline Func restart should receive one long string argument."));
                    return pending;
                }

                return std::make_shared<InlineFunc>(
                        *std::static_pointer_cast<LongStr>(*args.front())->get());
            }) {

    }

    void init(Constants &constants) {
        constants.add(Id{"", "func"}, std::make_shared<InlineFunc_Init>());
    }
}