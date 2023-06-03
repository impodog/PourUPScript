//
// Created by impodog on 6/3/2023.
//

#include "map_open.h"

namespace pups::library::builtins::map_open {
    Id id_mapOpen{"", "map"};

    MapOpen::MapOpen() : Function(
            [](FunctionArgs &args, Map *map) -> ObjectPtr {
                if (args.size() != 1 || !args.front()->get()->is_long_str()) {
                    map->throw_error(std::make_shared<ArgumentError>("Map opening must get one long str argument."));
                } else {
                    MapPtr sub_map = std::make_shared<Map>(map);
                    Control control(*std::static_pointer_cast<LongStr>(*args.front())->get(), sub_map);
                    control.run();
                    return sub_map;
                }
                return pending;
            }
    ) {

    }

    void init(Constants &constants) {
        constants.add(id_mapOpen, std::make_shared<MapOpen>());
    }

}
