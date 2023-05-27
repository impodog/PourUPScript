//
// Created by Dogs-Cute on 5/27/2023.
//

#include "error.h"

namespace pups::library {
    Error::Error(string s) : m_error(std::move(s)) {

    }

    string Error::get() const noexcept {
        return error_name() + ": " + m_error;
    }

    string TypeError::error_name() const noexcept {
        return "TypeError";
    }

    TypeError::TypeError(const string &s) : Error(s) {}

    string IdError::error_name() const noexcept {
        return "IdError";
    }

    IdError::IdError(const string &s) : Error(s) {}
}