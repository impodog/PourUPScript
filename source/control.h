//
// Created by Dogs-Cute on 5/27/2023.
//

#ifndef PUPS_LIB_TESTS_CONTROL_H
#define PUPS_LIB_TESTS_CONTROL_H

#include "map.h"

namespace pups::library {
    extern ObjectPtr sym_assign;

    class Constants {
    protected:
        ObjectMap constants;

        void load_from(const path &path);

    public:
        explicit Constants(const path &path);

        void export_to(Map *map);

        void add(const Id &id, const ObjectPtr &object);
    };

    class Control {
    protected:
        MapPtr m_map;
    public:
        IdFile idFile;
        const IdFactor *cur_id;
        size_t brace_depth = 0;
        bool is_new_line = false;

        explicit Control(const path &path, Constants &constants);

        explicit Control(const IdFile &idFile, MapPtr map);

        // Return if the id is empty(EOF => true)
        bool next_id();

        // Return if the id is empty(EOF => true)
        bool next_line();

        void run();
    };
}

#endif //PUPS_LIB_TESTS_CONTROL_H
