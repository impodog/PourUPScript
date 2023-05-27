//
// Created by Dogs-Cute on 5/27/2023.
//

#ifndef PUPS_LIB_TESTS_CONTROL_H
#define PUPS_LIB_TESTS_CONTROL_H

#include "map.h"
#include "builtins/numbers.h"

namespace pups::library {
    class Constants {
    protected:
        ObjectMap constants;
    public:
        explicit Constants(const path &path);

        void export_to(Map *map);
    };

    class Control {
    protected:
        Map global_map;
    public:
        IdFile idFile, then_end_idFile;
        Id cur_id;
        size_t then_depth = 0;

        explicit Control(const path &path);

        explicit Control(const string &str);

        explicit Control(const IdFile &idFile);

        // Return if the id is empty(EOF => true)
        bool next_id(bool &is_new_line);

        // Return if the id is empty(EOF => true)
        bool next_line();
    };
}

#endif //PUPS_LIB_TESTS_CONTROL_H
