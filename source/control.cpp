//
// Created by Dogs-Cute on 5/27/2023.
//

#include "control.h"

#include <utility>
#include <vector>

namespace pups::library {
    std::vector<MapPtr> global_maps;

    Control::Control(IdFile idFile, MapPtr map) :
            idFile(std::move(idFile)), cur_id(&this->idFile.get_id()), map(std::move(map)) {

    }

    Control::Control(const path &path, Constants &constants) :
            idFile(read_file(path)), cur_id(&idFile.get_id()),
            global_map(std::make_shared<Map>(path.parent_path())), map(std::make_shared<Map>(global_map.get(), false)) {
        global_maps.push_back(global_map);
        constants.export_to(global_map.get());
    }

    Control::Control(const path &path, Constants &constants, Map *map, bool allow_upsearch) :
            idFile(read_file(path)), cur_id(&idFile.get_id()),
            map(std::make_shared<Map>(map, path.parent_path(), allow_upsearch)) {
        constants.export_to(this->map->get_global());
    }

    Control::~Control() {
        map->restore();
        map->push_up_errs();
        map.reset();
    }

    bool Control::next_id() {
        //std::cout << "ID " << cur_id->str() << std::endl;
        if (!cur_id->empty() && map->can_run()) {
            if (cur_id->is_id()) {
                map->put(map->find(*cur_id->id(), map.get()), map.get());
            } else {
                map->put(map->add_to_memory_stack(std::make_shared<LongStr>(*cur_id->idFile())), map.get());
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

    void quit_global() {
        global_maps.clear();
    }
}