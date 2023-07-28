//
// Created by impodog on 6/3/2023.
//

#ifndef PUPS_LIB_CONSTANTS_H
#define PUPS_LIB_CONSTANTS_H

#include "map.h"

namespace pups::library {
    extern ObjectPtr sym_assign, sym_dynamic;

    class Constants {
    protected:
        enum Status {
            stat_none,
            stat_int,
            stat_float,
            stat_str
        } stat = stat_none;
        const std::unordered_map<std::string, Status> status_table = {
                {"int",   stat_int},
                {"float", stat_float},
                {"str",   stat_str}
        };
        using SubConstMap = IdMap<std::shared_ptr<Constants>>;
        ObjectMap constants;
        SubConstMap sub_consts;

        void run_line(const std::string &line);

        void switch_status(std::string status);

        void analyze_line(const std::string &line);

        virtual MapPtr make_sub_map(Map *map);

    public:
        Constants() = default;

        explicit Constants(const path &path);

        Constants(std::initializer_list<path> path);

        void load_from(const path &path);

        void export_to(Map *map);

        void add(const Id &id, const ObjectPtr &object);

        Constants &new_sub_const(const Id &id);
    };

    path get_constants_path(const path &path);
}

#endif //PUPS_LIB_CONSTANTS_H
