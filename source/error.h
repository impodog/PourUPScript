//
// Created by Dogs-Cute on 5/27/2023.
//

#ifndef PUPS_LIB_TESTS_ERROR_H
#define PUPS_LIB_TESTS_ERROR_H

#include "object.h"

namespace pups::library {
    class Error : public Object {
    protected:
        string m_error;

        [[nodiscard]] virtual string error_name() const noexcept = 0;

    public:
        ObjectPtr put(ObjectPtr &object, Map *map) override;

        explicit Error(string s);

        [[nodiscard]] string get() const noexcept;
    };

    using ErrorPtr = shared_ptr<Error>;

    class TypeError : public Error {
    protected:
        [[nodiscard]] string error_name() const noexcept override;

    public:
        explicit TypeError(const string &s);
    };

    class IdError : public Error {
    protected:
        [[nodiscard]] string error_name() const noexcept override;

    public:
        explicit IdError(const string &s);
    };

    class ArgumentError : public Error {
    protected:
        [[nodiscard]] string error_name() const noexcept override;

    public:
        explicit ArgumentError(const string &s);
    };
}

#endif //PUPS_LIB_TESTS_ERROR_H
