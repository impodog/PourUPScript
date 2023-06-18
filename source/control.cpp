//
// Created by Dogs-Cute on 5/27/2023.
//

#include "control.h"

namespace pups::library {
    Control::Control(const IdFile &idFile, MapPtr map) :
            idFile(idFile), cur_id(&idFile.get_id()), map(std::move(map)) {

    }

    Control::Control(const path &path, Constants &constants) :
            idFile(read_file(path)), cur_id(&idFile.get_id()),
            global_map(std::make_shared<Map>()), map(std::make_shared<Map>(global_map.get(), false)) {
        constants.export_to(global_map.get());
    }

    Control::Control(const path &path, Constants &constants, Map *map, bool allow_upsearch) :
            idFile(read_file(path)), cur_id(&idFile.get_id()), map(std::make_shared<Map>(map, allow_upsearch)) {
        constants.export_to(this->map.get());
    }

    Control::~Control() {
        if (map->get_parent())
            map->get_parent()->set_child(nullptr);
    }

    bool Control::next_id() {
        //std::cout << cur_id->str() << std::endl;
        if (!cur_id->empty() && map->can_run()) {
            if (cur_id->is_id()) {
                map->put(map->find(*cur_id->id(), map.get()), map.get());
            } else {
                Id id = generate_id();
                ObjectPtr object = std::make_shared<LongStr>(cur_id->idFile());
                map->add_object(id, object);
                map->put(object, map.get());
            }
        }

        cur_id = &idFile.next_id(is_new_line);
        if (is_new_line)
            map->end_of_line(map.get());
        return cur_id->empty();
    }

    bool Control::next_line() {
        is_new_line = false;
        while (!is_new_line && !next_id()) {
        }
        return cur_id->empty();
    }

    void Control::run() {
        while (!next_line()) {}
    }

    void Control::restart(MapPtr sub_map) {
        map = std::move(sub_map);
        idFile.restart();
        cur_id = &idFile.get_id();
    }

}