//
// Created by impodog on 6/3/2023.
//

#include "map_open.h"
#include "strings.h"

namespace pups::library::builtins::map_open {
    MapOpen::MapOpen() : Function(
            [](FunctionArgs &args, Map *map) -> ObjectPtr {
                if (args.empty() || !args.back()->get()->is_long_str()) {
                    map->throw_error(
                            std::make_shared<ArgumentError>("Map opening must find one long str argument in the back."));
                    return pending;
                }
                MapPtr sub_map = std::make_shared<Map>(map);
                Control control(*std::static_pointer_cast<LongStr>(*args.back())->ids(), sub_map);
                control.run();
                while (args.size() != 1) {
                    *args.front() = sub_map;
                    args.pop();
                }
                return sub_map;
            }
    ) {}

    ModuleOpen::ModuleOpen() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        if (args.size() != 1) {
            map->throw_error(std::make_shared<ArgumentError>("Module opening must find one only string argument."));
            return pending;
        }
        auto ptr = std::dynamic_pointer_cast<strings::String>(*args.front());
        if (ptr) {
            path p = ptr->data();
            if (std::filesystem::exists(p)) {
                path const_path = p.parent_path().append(p.stem().string() + ".con");
                Constants constants(const_path);
                Control control(p, constants, map);
                control.run();
                return control.map;
            } else {
                map->throw_error(std::make_shared<FileNotFoundError>("File \"" + ptr->data() + "\" is not found."));
                return pending;
            }
        } else {
            map->throw_error(std::make_shared<ArgumentError>("Module opening must find a string argument."));
            return pending;
        }
    }) {}

    MapWith::MapWith() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        if (args.size() != 2) {
            map->throw_error(std::make_shared<ArgumentError>(
                    "Map-with statement must find a map argument and long string argument."));
            return pending;
        }
        auto sub_map = std::dynamic_pointer_cast<Map>(*args.front());
        args.pop();
        auto ids = std::dynamic_pointer_cast<LongStr>(*args.front());
        args.pop();
        if (sub_map && ids) {
            sub_map->get_parent()->set_child(sub_map.get());
            Control control(*ids->ids(), sub_map);
            control.run();
            return sub_map;
            // Naturally the child would be cleaned up by Control
        } else {
            map->throw_error(std::make_shared<ArgumentError>(
                    "Map-with statement must find a map argument and a long string argument."));
            return pending;
        }
    }) {}


    Id id_mapOpen{"", "map"}, id_moduleOpen{"", "module"}, id_mapWith{"", "with"};

    void init(Constants &constants) {
        constants.add(id_mapOpen, std::make_shared<MapOpen>());
        constants.add(id_moduleOpen, std::make_shared<ModuleOpen>());
        constants.add(id_mapWith, std::make_shared<MapWith>());

    }
}
