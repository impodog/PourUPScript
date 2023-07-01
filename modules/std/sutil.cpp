//
// Created by impodog on 7/1/2023.
//

#include "sutil.h"

namespace pups::modules::sutil {
    using OnlyStringFunction = std::function<ObjectPtr(const std::string &)>;

    ObjectPtr string_function(const OnlyStringFunction &core) {
        return std::make_shared<Function>([core](FunctionArgs &args, Map *map) -> ObjectPtr {
            if (args.size() != 1)
                map->throw_error(
                        std::make_shared<library::ArgumentError>("The string function requires one only argument."));
            else {
                auto ptr = cast<strings::String>(*args.front());
                if (ptr) {
                    try {
                        return core(ptr->data());
                    } catch (const std::exception &e) {
                        map->throw_error(std::make_shared<library::ValueError>(
                                "String function using " + ptr->repr() + " failed with " + e.what() + "."));
                    }
                } else
                    map->throw_error(
                            std::make_shared<library::TypeError>("The string function requires a string argument."));
            }
            return pending;
        });
    }

    Id id_sutil{"", "sutil"};
    Id id_to_int{"", "toi"}, id_to_float{"", "tof"};

    void init(Constants &constants) {
        auto &sutil = constants.new_sub_const(id_sutil);
        sutil.add(id_to_int, string_function([](const std::string &s) -> ObjectPtr {
            return std::make_shared<numbers::IntType>(std::stoll(s));
        }));
        sutil.add(id_to_float, string_function([](const std::string &s) -> ObjectPtr {
            return std::make_shared<numbers::FloatType>(std::stod(s));
        }));
    }
}