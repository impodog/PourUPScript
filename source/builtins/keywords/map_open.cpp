//
// Created by impodog on 6/3/2023.
//

#include "map_open.h"
#include "../types/strings.h"

namespace pups::library::builtins::map_open {
    std::unordered_map<std::string, MapPtr> opened_modules;
    path module_path = ".";

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
                    args.pop_front();
                }
                return sub_map;
            }
    ) {}

    ObjectPtr open_module_link(const std::string &name, Map *map) {
        try {
            return opened_modules.at(name);
        } catch (const std::out_of_range &) {}

        auto tmp_name = module_link_name(name);
        auto result = map->find(tmp_name, map);
        if (is_pending(result))
            throw std::invalid_argument("Module link not found.");
        else {
            auto ptr = cast<Function>(result);
            if (ptr) {
                MapPtr new_map = std::make_shared<Map>(map->get_global(), false);
                map->add_object(Id{"", name}, new_map);
                ptr->end_of_line(new_map.get());
                map->set_child(nullptr);
                opened_modules.insert({name, new_map});
                return new_map;
            } else
                map->throw_error(
                        std::make_shared<TypeError>("Module link \"" + tmp_name.str() + "\" is not a function."));
            return pending;
        }
    }

    ObjectPtr open_file(const std::string &name, Map *map) {
        path p = module_path;
        p /= name;
        p = std::filesystem::absolute(p);
        try {
            return opened_modules.at(name);
        } catch (const std::out_of_range &) {}

        if (std::filesystem::exists(p)) {
            path const_path = p.parent_path().append(p.stem().string() + ".con");
            Constants constants(const_path);
            Control control(p, constants, map->get_global(), false);
            control.run();
            opened_modules.insert({p.string(), control.map});
            return control.map;
        } else {
            map->throw_error(std::make_shared<FileNotFoundError>("When opening file, \"" + name + "\" is not found."));
            return pending;
        }
    }

    ModuleOpen::ModuleOpen() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        if (args.size() != 1) {
            map->throw_error(std::make_shared<ArgumentError>("Module opening must find one only string argument."));
            return pending;
        }
        auto ptr = cast<strings::String>(*args.front());
        if (ptr) {
            /*
            try {
                return open_module_link(ptr->data(), map);
            } catch (const std::invalid_argument &) {
                return open_file(ptr->data(), map);
            }
            */
            return open_file(ptr->data(), map);
        } else {
            map->throw_error(std::make_shared<TypeError>("Module opening must find a string argument."));
            return pending;
        }
    }) {}

    MapEnter::MapEnter() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        if (args.size() != 2) {
            map->throw_error(std::make_shared<ArgumentError>(
                    "Map-enter statement requires two arguments."));
            return pending;
        }
        auto sub_map = cast<Map>(*args.front());
        args.pop_front();
        auto ids = cast<LongStr>(*args.front());
        args.pop_front();
        if (sub_map && ids) {
            sub_map->get_parent()->set_child(sub_map.get());
            Control control(*ids->ids(), sub_map);
            control.run();
            return sub_map;
            // Naturally the child would be cleaned up by Control
        } else {
            map->throw_error(std::make_shared<TypeError>(
                    "Map-enter statement requires a map argument and a long string argument."));
            return pending;
        }
    }) {}

    Catch::Catch() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
        if (args.empty()) {
            map->throw_error(std::make_shared<ArgumentError>(
                    "Catch statement requires at least one argument."));
            return pending;
        } else {
            CatchRequirements required;
            auto ids = cast<LongStr>(*args.back());
            args.pop_back();
            if (ids) {
                while (!args.empty()) {
                    auto ptr = cast<strings::String>(*args.front());
                    if (ptr)
                        required.insert(ptr->data());
                    else
                        map->throw_error(std::make_shared<TypeError>(
                                "Catch statement requires string arguments instead of" + args.front()->get()->repr() +
                                ". Argument ignored."));
                    args.pop_front();
                }
                if (map->catch_by(required)) {
                    MapPtr sub_map = std::make_shared<Map>(map, true);
                    Control control(*ids->ids(), sub_map);
                    control.run();
                    return sub_map;
                }
            } else
                map->throw_error(
                        std::make_shared<TypeError>("Catch statement requires a long string argument in the back."));
        }
        return pending;
    }) {}


    Id id_mapOpen{"", "map"}, id_moduleOpen{"", "import"}, id_mapEnter{"", "enter"}, id_catch{"", "catch"};

    void init(Constants &constants) {
        constants.add(id_mapOpen, std::make_shared<MapOpen>());
        constants.add(id_moduleOpen, std::make_shared<ModuleOpen>());
        constants.add(id_mapEnter, std::make_shared<MapEnter>());
        constants.add(id_catch, std::make_shared<Catch>());
    }

    void quit() {
        opened_modules.clear();
    }

    void set_module_path(path path) {
        module_path = std::move(path);
    }

}
