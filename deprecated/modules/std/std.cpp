//
// Created by impodog on 6/18/2023.
//

#include "std.h"
#include "random.h"
#include "string_code.h"
#include "type_judge.h"
#include "memory.h"
#include "sys.h"
#include "ios.h"
#include "sutil.h"
#include "pupsmath.h"

namespace pups::modules::pups_std {
    void init(Constants &constants) {
        random::init(constants);
        string_code::init(constants);
        type_judge::init(constants);
        memory::init(constants);
        sys::init(constants);
        ios::init(constants);
        sutil::init(constants);
        math::init(constants);
    }

    Id get_std_func_name(const std::string &name) {
        return Id{"", STD_NAME + name + "_load"};
    }

    Id get_std_lib_name(const std::string &name) {
        return Id{"", STD_NAME + name};
    }
}

extern "C" void PUPS_Init_pups_std(pups::PackageHandler &handler) {
    // PackageHandler is Constants? This is not intended to be used like this.
    pups::modules::pups_std::init(handler);
}
