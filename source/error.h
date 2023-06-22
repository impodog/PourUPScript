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

        [[nodiscard]] virtual std::string error_name() const noexcept = 0;

    public:
        ObjectPtr put(ObjectPtr &object, Map *map) override;

        explicit Error(std::string s);

        [[nodiscard]] std::string get() const noexcept;
    };

    using ErrorPtr = std::shared_ptr<Error>;

    class TypeError : public Error {
    protected:
        [[nodiscard]] std::string error_name() const noexcept override;

    public:
        explicit TypeError(const std::string &s);
    };

    class IdError : public Error {
    protected:
        [[nodiscard]] std::string error_name() const noexcept override;

    public:
        explicit IdError(const std::string &s);
    };

    class ArgumentError : public Error {
    protected:
        [[nodiscard]] std::string error_name() const noexcept override;

    public:
        explicit ArgumentError(const std::string &s);
    };

    class FileNotFoundError : public Error {
    protected:
        [[nodiscard]] std::string error_name() const noexcept override;

    public:
        explicit FileNotFoundError(const std::string &s);
    };

    class ValueError : public Error {
    protected:
        [[nodiscard]] std::string error_name() const noexcept override;

    public:
        explicit ValueError(const std::string &s);
    };

    class OutOfBoundError : public Error {
    protected:
        [[nodiscard]] std::string error_name() const noexcept override;

    public:
        explicit OutOfBoundError(const std::string &s);
    };
}

#endif //PUPS_LIB_ERROR_H
