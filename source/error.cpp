//
// Created by Dogs-Cute on 5/27/2023.
//

#include "error.h"

namespace pups::library {
    Error::Error(std::string s) : m_error(std::move(s)) {

    }

    std::string Error::get() const noexcept {
        return error_name() + ": " + m_error;
    }

    std::string TypeError::error_name() const noexcept {
        return "TypeError";
    }

    TypeError::TypeError(const std::string &s) : Error(s) {}

    std::string IdError::error_name() const noexcept {
        return "IdError";
    }

    IdError::IdError(const std::string &s) : Error(s) {}

    std::string ArgumentError::error_name() const noexcept {
        return "ArgumentError";
    }

    ArgumentError::ArgumentError(const std::string &s) : Error(s) {}

    std::string FileNotFoundError::error_name() const noexcept {
        return "FileNotFoundError";
    }

    FileNotFoundError::FileNotFoundError(const std::string &s) : Error(s) {}

    std::string ValueError::error_name() const noexcept {
        return "ValueError";
    }

    ValueError::ValueError(const std::string &s) : Error(s) {}

    std::string OutOfBoundError::error_name() const noexcept {
        return "OutOfBoundError";
    }

    OutOfBoundError::OutOfBoundError(const std::string &s) : Error(s) {}
}