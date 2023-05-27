//
// Created by Dogs-Cute on 5/27/2023.
//

#include "control.h"

namespace pups::library {
    Constants::Constants(const path &path) {

    }

    void Constants::export_to(Map *map) {
        for (auto &constant: constants) {
            map->add_object(constant.first, constant.second);
        }
    }

    Control::Control(const path &path) : idFile(read_file(path)), cur_id(idFile.get_id()) {}

    Control::Control(const string &str) : idFile(read_string(str)), cur_id(idFile.get_id()) {}

    Control::Control(const IdFile &idFile) : idFile(idFile), cur_id(idFile.get_id()) {

    }

    bool Control::next_id(bool &is_new_line) {
        if (cur_id.id() == "then")
            then_depth++;
        else if (cur_id.id() == "end")
            then_depth--;
        else if (then_depth == 0) {
            if (then_end_idFile.empty())
                global_map.put(global_map.get(cur_id), &global_map);
            else {
                Id id = generate_id();
                ObjectPtr object = std::make_shared<LongStr>(then_end_idFile);
                global_map.put(object, &global_map);
                global_map.end_of_line(&global_map);
                is_new_line = true;
            }
        } else {
            then_end_idFile.add_id(cur_id);
            if (is_new_line)
                then_end_idFile.new_line();
        }

        cur_id = idFile.next_id(is_new_line);
        if (is_new_line)
            global_map.end_of_line(&global_map);
        return cur_id.empty();
    }

    bool Control::next_line() {
        bool is_new_line = false;
        while (!is_new_line && !next_id(is_new_line)) {}
        return cur_id.empty();
    }

}