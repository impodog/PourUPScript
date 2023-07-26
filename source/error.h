//
// Created by Dogs-Cute on 5/27/2023.
//

#ifndef PUPS_LIB_ERROR_H
#define PUPS_LIB_ERROR_H

#include "object.h"

namespace pups::library {
    class Error : public Object {
    protected:
        std::string m_error;

        friend class Map;

    public:
        ObjectPtr put(ObjectPtr &object, Map *map) override;

        explicit Error(std::string s);

        [[nodiscard]] virtual std::string error_name() const noexcept = 0;

        [[nodiscard]] std::string get() const noexcept;
    };

    using ErrorPtr = std::shared_ptr<Error>;

    class TypeError : public Error {
    public:
        [[nodiscard]] std::string error_name() const noexcept override;

        explicit TypeError(const std::string &s);
    };

    class IdError : public Error {
    public:
        [[nodiscard]] std::string error_name() const noexcept override;

        explicit IdError(const std::string &s);
    };

    class ArgumentError : public Error {
    public:
        [[nodiscard]] std::string error_name() const noexcept override;

        explicit ArgumentError(const std::string &s);
    };

    class FileNotFoundError : public Error {
    public:
        [[nodiscard]] std::string error_name() const noexcept override;

        explicit FileNotFoundError(const std::string &s);
    };

    class ValueError : public Error {
    public:
        [[nodiscard]] std::string error_name() const noexcept override;

        explicit ValueError(const std::string &s);
    };

    class OutOfBoundError : public Error {
    public:
        [[nodiscard]] std::string error_name() const noexcept override;

        explicit OutOfBoundError(const std::string &s);
    };

    class ScriptError : public Error {
    public:
        [[nodiscard]] std::string error_name() const noexcept override;

        explicit ScriptError(const std::string &s);
    };

    class PUPS_Exit final : public std::exception {
    public:
        PUPS_Exit();
    };
}

#endif //PUPS_LIB_ERROR_H
