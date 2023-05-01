//
// Created by Dogs-Cute on 5/1/2023.
//

#ifndef POURUPSCRIPTINTERP_UTIL_HPP
#define POURUPSCRIPTINTERP_UTIL_HPP

namespace PUPS {
    template<typename Type>
    void swap(Type &x, Type &y) {
        Type mid = x;
        x = y;
        y = mid;
    }

    template<typename Type>
    void swap_ptr_data(Type *x, Type *y) {
        Type mid = *x;
        *x = *y;
        *y = mid;
    }
}

#endif //POURUPSCRIPTINTERP_UTIL_HPP
