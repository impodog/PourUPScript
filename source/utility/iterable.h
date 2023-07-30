//
// Created by impodog on 7/30/2023.
//

#ifndef PUPS_LIB_ITERABLE_H
#define PUPS_LIB_ITERABLE_H

#include "const.h"

namespace pups::library::utility {
    using namespace builtins;
    using namespace function;
    using containers::Deque;

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

    template<typename IterType>
    auto &get_data(IterType &iter) {
        return iter.data;
    }

    template<>
    inline auto &get_data<strings::String>(strings::String &iter) {
        return iter.data();
    }

    template<typename IterType>
    using data_type_of = std::remove_reference_t<decltype(get_data(std::declval<IterType &>()))>;

    template<typename IterType>
    data_type_of<IterType> create_sized(size_t size) {
        return data_type_of<IterType>(size);
    }

    template<>
    inline data_type_of<Deque> create_sized<Deque>(size_t size) {
        data_type_of<Deque> result;
        result.resize(size);
        return result;
    }
}

#endif //PUPS_LIB_ITERABLE_H
