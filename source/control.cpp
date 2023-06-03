//
// Created by Dogs-Cute on 5/27/2023.
//

#include "control.h"

namespace pups::library {
    Control::Control(const IdFile &idFile, MapPtr map) :
            idFile(idFile), cur_id(&idFile.get_id()), m_map(std::move(map)) {

    }

    Control::Control(const path &path, Constants &constants) :
            idFile(read_file(path)), cur_id(&idFile.get_id()), m_map(std::make_shared<Map>()) {
        constants.export_to(m_map.get());
    }

    bool Control::next_id() {
        //std::cout << cur_id->str() << std::endl;
        if (!cur_id->empty()) {
            if (cur_id->is_id()) {
                m_map->put(m_map->get(*cur_id->id()), m_map.get());
            } else {
                Id id = generate_id();
                ObjectPtr object = std::make_shared<LongStr>(cur_id->idFile());
                m_map->add_object(id, object);
                m_map->put(object, m_map.get());
            }
        }

        cur_id = &idFile.next_id(is_new_line);
        if (is_new_line)
            m_map->end_of_line(m_map.get());
        return cur_id->empty();
    }

    bool Control::next_line() {
        is_new_line = false;
        while (!is_new_line && !next_id()) {
        }
        return cur_id->empty();
    }

    void Control::run() {
        while (!next_line()) {
            //std::cout << pending << std::endl;
        }
    }

    void Control::restart(MapPtr map) {
        m_map = std::move(map);
        idFile.restart();
        cur_id = &idFile.get_id();
    }

}