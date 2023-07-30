//
// Created by impodog on 7/30/2023.
//

#ifndef PUPS_LIB_ITERABLE_H
#define PUPS_LIB_ITERABLE_H

#include "../control.h"
#include "../builtins/builtins.h"

namespace pups::library::utility {
    using namespace builtins;
    using namespace function;

    template<typename ParserType>
    ObjectPtr work_with_iter(FunctionArgs &args, Map *map) {
        auto ptr = cast<typename ParserType::type>(*args.front());
        if (ptr) {
            return ParserType::work(*ptr, args, map);
        } else {
            throw std::invalid_argument("Type working failure.");
        }
    }

    template<typename ParserType, typename NextType, typename ...types>
    ObjectPtr work_with_iter(FunctionArgs &args, Map *map) {
        try {
            return work_with_iter<ParserType>(args, map);
        } catch (const std::invalid_argument &) {
            return work_with_iter<NextType, types...>(args, map);
        }
    }

    template<typename IterType, typename DataType>
    DataType &get_data(IterType &iter) {
        return iter.data;
    }

    template<>
    inline std::string &get_data<strings::String, std::string>(strings::String &iter) {
        return iter.data();
    }
}

#endif //PUPS_LIB_ITERABLE_H
