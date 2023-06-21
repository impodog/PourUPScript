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
                            std::make_shared<ArgumentError>(
                                    "Map opening must find one long str argument in the back."));
                    return pending;
                }
                MapPtr sub_map = std::make_shared<Map>(map, false);
                Control control(*std::static_pointer_cast<LongStr>(*args.back())->ids(), sub_map);
                control.run();
                while (args.size() != 1) {
                    *args.front() = sub_map;
                    args.pop();
                }
                return sub_map;
            }
    ) {}

    ObjectPtr open_module_link(const std::string &name, Map *map) {
        auto tmp_name = module_link_name(name);
        auto result = map->find(tmp_name, map);
        if (is_pending(result))
            throw std::invalid_argument("Module link not found.");
        else {
            auto ptr = std::dynamic_pointer_cast<Function>(result);
            if (ptr) {
                MapPtr new_map = std::make_shared<Map>(map, false);
                map->add_object(Id{"", name}, new_map);
                ptr->end_of_line(new_map.get());
                map->set_child(nullptr);
                return new_map;
            } else
                map->throw_error(
                        std::make_shared<TypeError>("Module link \"" + tmp_name.str() + "\" is not a function."));
            return pending;
        }
    }

    ObjectPtr open_file(const std::string &name, Map *map) {
        path p = name;
        if (std::filesystem::exists(p)) {
            path const_path = p.parent_path().append(p.stem().string() + ".con");
            Constants constants(const_path);
            Control control(p, constants, map, false);
            control.run();
            return control.map;
        } else {
            map->throw_error(std::make_shared<FileNotFoundError>("File \"" + name + "\" is not found."));
            return pending;
        }
    }

    ModuleOpen::ModuleOpen() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        if (args.size() != 1) {
            map->throw_error(std::make_shared<ArgumentError>("Module opening must find one only string argument."));
            return pending;
        }
        auto ptr = std::dynamic_pointer_cast<strings::String>(*args.front());
        if (ptr) {
            try {
                return open_module_link(ptr->data(), map);
            } catch (const std::invalid_argument &) {
                return open_file(ptr->data(), map);
            }
        } else {
            map->throw_error(std::make_shared<ArgumentError>("Module opening must find a string argument."));
            return pending;
        }
    }) {}

    MapEnter::MapEnter() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        if (args.size() != 2) {
            map->throw_error(std::make_shared<ArgumentError>(
                    "Map-enter statement must find a map argument and long string argument."));
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
                    "Map-enter statement must find a map argument and a long string argument."));
            return pending;
        }
    }) {}


    Id id_mapOpen{"", "map"}, id_moduleOpen{"", "imp"}, id_mapEnter{"", "enter"};

    void init(Constants &constants) {
        constants.add(id_mapOpen, std::make_shared<MapOpen>());
        constants.add(id_moduleOpen, std::make_shared<ModuleOpen>());
        constants.add(id_mapEnter, std::make_shared<MapEnter>());
    }
}
