//
// Created by impodog on 7/25/2023.
//

#include "debug_info.h"

namespace pups::library::builtins::debug_info {
    DebugInfo::DebugInfo() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        map->change_debug_info(strings::str_of(args));
        return map->get_temp();
    }) {}

    Id id_debugInfo{"", "DEBUG_INFO"};

    void init(Constants &constants) {
        constants.add(id_debugInfo, std::make_shared<DebugInfo>());
    }
}
