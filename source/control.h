//
// Created by Dogs-Cute on 5/27/2023.
//

#ifndef PUPS_LIB_TESTS_CONTROL_H
#define PUPS_LIB_TESTS_CONTROL_H

#include "constants.h"

namespace pups::library {
    class Control {
    protected:
        MapPtr m_map;
    public:
        IdFile idFile;
        const IdFactor *cur_id;
        bool is_new_line = false;

        explicit Control(const path &path, Constants &constants);

        explicit Control(const IdFile &idFile, MapPtr map);

        // Return if the id is empty(EOF => true)
        bool next_id();

        // Return if the id is empty(EOF => true)
        bool next_line();

        void run();

        void restart(MapPtr map);
    };
}

#endif //PUPS_LIB_TESTS_CONTROL_H
