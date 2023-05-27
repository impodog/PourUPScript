//
// Created by Dogs-Cute on 5/27/2023.
//

#ifndef PUPS_EZN_SYNTAX_H
#define PUPS_EZN_SYNTAX_H

#include "const.h"

namespace pups::library {
    class Id {
    protected:
        std::string m_qual, m_id;
    public:
        Id() = default;

        Id(std::string m_qual, std::string id);

        [[nodiscard]] const std::string &qual() const;

        [[nodiscard]] bool qual_has(char q) const;

        [[nodiscard]] const std::string &id() const;

        [[nodiscard]] std::string str() const;

        [[nodiscard]] bool empty() const noexcept;

        [[nodiscard]] bool lf() const noexcept;

        bool operator==(const Id &rhs) const;

        struct hash {
            size_t operator()(const Id &id) const;
        };
    };

    extern Id empty_id;

    class IdFile {
    protected:
        using Line = std::vector<Id>;
        using File = std::vector<Line>;
        File m_file = {{}};
        std::pair<size_t, size_t> m_cursor = {0, 0};

        bool next_cursor();

    public:
        IdFile() = default;

        [[nodiscard]] const Id &get_id() const;

        const Id &next_id(bool &is_new_line);

        void add_id(const Id &id);

        void new_line();

        bool empty() const noexcept;

        void clear() noexcept;
    };

    IdFile read_file(const path &path);

    IdFile read_string(string s);

    // Generates a different, minimal id each time.
    Id generate_id();
}

#endif //PUPS_EZN_SYNTAX_H
