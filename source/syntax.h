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

        explicit Id(std::string qual, std::string id);

        explicit Id(const std::string &qual_id);

        [[nodiscard]] const std::string &qual() const;

        [[nodiscard]] bool qual_has(char q) const;

        [[nodiscard]] const std::string &id() const;

        [[nodiscard]] std::string str() const;

        [[nodiscard]] std::queue<std::string> split_by(char sep) const;

        [[nodiscard]] bool empty() const noexcept;

        [[nodiscard]] bool lf() const noexcept;

        [[nodiscard]] bool lbrace() const noexcept;

        [[nodiscard]] bool rbrace() const noexcept;

        bool operator==(const Id &rhs) const;

        bool operator==(const std::string &rhs) const;

        struct hash {
            size_t operator()(const Id &id) const;
        };
    };

    class IdFile;

    using IdPtr = std::shared_ptr<Id>;
    using IdFilePtr = std::shared_ptr<IdFile>;

    class IdFactor {
        enum Type {
            t_id, t_idFile
        } const m_type;
        IdPtr m_id;
        IdFilePtr m_idFile;
    public:
        explicit IdFactor(IdPtr id);

        explicit IdFactor(IdFilePtr idFile);

        ~IdFactor() = default;

        [[nodiscard]] bool is_id() const noexcept;

        [[nodiscard]] bool empty() const noexcept;

        [[nodiscard]] const IdPtr &id() const noexcept;

        [[nodiscard]] const IdFilePtr &idFile() const noexcept;

        [[nodiscard]] std::string str() const noexcept;
    };

    extern IdPtr empty_id;
    extern IdFactor empty_factor;

    class IdFile {
    protected:
        using Line = std::vector<IdFactor>;
        using File = std::vector<Line>;
        using FilePtr = std::shared_ptr<File>;
        FilePtr m_file;
        std::pair<size_t, size_t> m_cursor = {0, 0};

        bool next_cursor();

    public:
        IdFile();

        [[nodiscard]] const IdFactor &get_id() const;

        const IdFactor &next_id(bool &is_new_line);

        void add_id(const IdPtr &id);

        void add_idFile(const IdFilePtr &idFile);

        void new_line();

        [[nodiscard]] bool empty() const noexcept;

        void clear() noexcept;

        void restart() noexcept;

        [[nodiscard]] Line all() const noexcept;

        [[nodiscard]] std::string str() const noexcept;
    };

    template<typename ValueType>
    using IdMap = std::unordered_map<Id, ValueType, Id::hash>;
    using IdSet = std::unordered_set<Id, Id::hash>;

    IdFile read_file(const path &path);

    IdFile read_string(std::string s);

    using SyntaxFunc = std::function<char()>;

    IdFile read_block(const SyntaxFunc &func, const SyntaxFunc &peek);

    // Generates a different, minimal id each time.
    Id generate_id();

    std::string to_string(const void *ptr);

    Id spec_name_of(const std::string &name);
}

#endif //PUPS_EZN_SYNTAX_H
