//
// Created by Dogs-Cute on 5/27/2023.
//

#ifndef PUPS_LIB_CONTROL_H
#define PUPS_LIB_CONTROL_H

#include "constants.h"

namespace pups::library {
    class Control {
    public:
        MapPtr global_map = nullptr, map;
        IdFile idFile;
        const IdFactor *cur_id;
        bool is_new_line = false;

        explicit Control(const path &path, Constants &constants);

        explicit Control(const path &path, Constants &constants, Map *map, bool allow_upsearch);

        explicit Control(const IdFile &idFile, MapPtr map);

        ~Control();

        // Return if the id is empty(EOF => true)
        bool next_id();

        // Return if the id is empty(EOF => true)
        bool next_line();

        void run();

        void restart(MapPtr sub_map);
    };

    using ControlPtr = std::shared_ptr<Control>;
}

#endif //PUPS_LIB_CONTROL_H
