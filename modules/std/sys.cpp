//
// Created by impodog on 6/18/2023.
//

#include "sys.h"

namespace pups::modules::sys {
    using namespace pups::library::builtins::function;
    using namespace pups::library::builtins;
    
    ObjectPtr is_toplevel(FunctionArgs &args, Map *map) {
        return (map->get_parent() && map->get_parent()->get_parent() == nullptr)
                ? numbers::True : numbers::False;
    }

    Id id_is_toplevel{"", "is_toplevel"}, id_version{"", "version"};

    ObjectPtr sys_load(FunctionArgs &args, Map *map) {
        map->add_object(id_is_toplevel, std::make_shared<Function>(is_toplevel));
        map->add_object(id_version, std::make_shared<String>(PUPS_LIB_VERSION));
        return pending;
    }

    void init(pups::Constants &constants) {
        auto sys_func = std::make_shared<Function>(sys_load);
        constants.add(pups_std::get_std_func_name("sys"), sys_func);
        constants.add(module_link_name("sys"), sys_func);
    }
}