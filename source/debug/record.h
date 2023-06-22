//
// Created by impodog on 6/3/2023.
//

#ifndef PUPS_LIB_RECORD_H
#define PUPS_LIB_RECORD_H

#include "../const.h"

namespace pups::debug {

    inline std::string view_char(char c) {
        switch (c) {
            case '\n':
                return "'\\n'";
            case '\t':
                return "'\\t'";
            case ' ':
                return "SPACE";
            case EOF:
                return "EOF";
            default:
                return {'\'', c, '\''};
        }
    }

    template<typename Type>
    class Record {
        using ptr = std::unique_ptr<Type>;
        ptr m_value;
    public:
        explicit Record(ptr value) : m_value(std::move(value)) {}

        Record(const Type &value) : Record(std::make_unique<Type>(value)) {} // NOLINT(google-explicit-constructor)

        Record &operator=(const Type &value) {
            std::cout << "Data " << reinterpret_cast<void *>(m_value.get()) << ", changing ";
            if constexpr (std::is_same<Type, char>::value)
                std::cout << view_char(*m_value) << " to " << view_char(value) << std::endl;
            else
                std::cout << m_value << " to " << value << std::endl;
            m_value = std::make_unique<Type>(value);
            return *this;
        }

        operator Type &() { // NOLINT(google-explicit-constructor)
            return *m_value;
        }

        operator const Type &() const { // NOLINT(google-explicit-constructor)
            return *m_value;
        }
    };
}

#endif //PUPS_LIB_RECORD_H
