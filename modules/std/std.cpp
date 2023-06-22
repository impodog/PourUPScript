//
// Created by impodog on 6/18/2023.
//

#include "std.h"
#include "random.h"
#include "convert.h"
#include "string_code.h"
#include "type_judge.h"

namespace pups::modules::pups_std {
    void init(Constants &constants) {
        random::init(constants);
        convert::init(constants);
        string_code::init(constants);
        type_judge::init(constants);
    }

    Id get_std_func_name(const std::string &name) {
        return Id{"", STD_NAME + name + "_load"};
    }
}