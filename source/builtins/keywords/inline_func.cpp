//
// Created by Dogs-Cute on 5/28/2023.
//

#include "inline_func.h"

namespace pups::library::builtins::inline_func {
    constexpr const char *name_packet = "pack";
    Id id_arg_catch{"", "param"}, id_param_packet{"", name_packet};
    ObjectPtr sym_packet = std::make_shared<Symbol>(name_packet);

    Arguments::Arguments(FunctionArgs args) : args(std::move(args)) {

    }

    ObjectPtr Arguments::put(ObjectPtr &object, Map *map) {
        if (args.empty())
            map->throw_error(
                    std::make_shared<ArgumentError>("Not enough arguments given in a function call."));
        else {
            if (object.get() == sym_packet.get())
                packet_mode = true;
            else {
                if (packet_mode) {
                    std::vector<ObjectPtr> array;
                    while (!args.empty()) {
                        array.push_back(*args.front());
                        args.pop_front();
                    }
                    object = std::make_shared<containers::Array>(array);
                } else {
                    object = *args.front();
                    args.pop_front();
                }
            }
        }
        return nullptr;
    }

    void InlineFunc::add_arguments(MapPtr &map, const FunctionArgs &args) {
        map->add_object(id_arg_catch, std::make_shared<Arguments>(args));
    }

    InlineFunc::InlineFunc(IdFile idFile, Map *m_static_link) :
            m_idFile(std::move(idFile)), m_static_link(m_static_link),
            Function([this](const FunctionArgs &args, Map *map) -> ObjectPtr {
                MapPtr sub_map;
                if (this->m_static_link)
                    sub_map = std::make_shared<Map>(this->m_static_link, map, false); // Setting restore map
                else
                    sub_map = std::make_shared<Map>(map, false);
                Control control(m_idFile, sub_map);
                add_arguments(sub_map, args);
                control.run();
                auto ret = sub_map->get_return();
                return ret ? ret : pending;
            }) {

    }

    bool InlineFunc::is_process() const noexcept {
        return this->m_static_link == nullptr;
    }

    template<bool is_process>
    class InlineFunc_Init : public Function {
    public:
        InlineFunc_Init() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
            if (args.empty() || !args.back()->get()->is_long_str()) {
                map->throw_error(std::make_shared<ArgumentError>(
                        "Inline Func initialization should receive one long str argument in the back."));
                return pending;
            }
            auto func = std::make_shared<InlineFunc>(
                    std::static_pointer_cast<LongStr>(*args.back())->ids(),
                    is_process ? nullptr : map);
            while (args.size() != 1) {
                *args.front() = func;
                args.pop_front();
            }
            return pending;
        }) {

        }
    };

    Id id_funcInit{"", "func"}, id_procInit{"", "proc"};

    void init(Constants &constants) {
        constants.add(id_funcInit, std::make_shared<InlineFunc_Init<false>>());
        constants.add(id_procInit, std::make_shared<InlineFunc_Init<true>>());
        constants.add(id_param_packet, sym_packet);
    }
}