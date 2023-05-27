//
// Created by Dogs-Cute on 5/27/2023.
//

#include "syntax.h"

namespace pups::library {

    Id empty_id;

    Id::Id(std::string m_qual, std::string id) : m_qual(std::move(m_qual)), m_id(std::move(id)) {

    }

    const std::string &Id::qual() const {
        return m_qual;
    }

    bool Id::qual_has(char q) const {
        return m_qual.find(q) != std::string::npos;
    }

    const std::string &Id::id() const {
        return m_id;
    }

    std::string Id::str() const {
        return m_qual + m_id;
    }

    bool Id::empty() const noexcept {
        return m_id.empty();
    }

    bool Id::lf() const noexcept {
        return m_id.front() == '\n';
    }

    bool Id::operator==(const Id &rhs) const {
        return m_id == rhs.m_id;
    }

    size_t Id::hash::operator()(const Id &id) const {
        return std::hash<std::string>()(id.m_id);
    }

    bool IdFile::next_cursor() {
        if (m_cursor.second + 1 < m_file.at(m_cursor.first).size()) {
            m_cursor.second++;
            return false;
        } else {
            m_cursor.first++;
            m_cursor.second = 0;
            return true;
        }
    }

    const Id &IdFile::get_id() const {
        if (m_cursor.first == m_file.size())
            return empty_id;
        try {
            return m_file.at(m_cursor.first).at(m_cursor.second);
        } catch (const std::out_of_range &) {
            return empty_id;
        }
    }

    const Id &IdFile::next_id(bool &is_new_line) {
        is_new_line = next_cursor();
        return get_id();
    }

    void IdFile::add_id(const Id &id) {
        m_file.back().push_back(id);
    }

    void IdFile::new_line() {
        m_file.emplace_back();
    }

    bool IdFile::empty() const noexcept {
        return m_file.empty() || (m_file.size() == 1 && m_file.front().empty());
    }

    void IdFile::clear() noexcept {
        m_file.clear();
    }

    bool is_qualifier(char c) {
        return c == '$' || c == '&' || c == '%' || c == '\"';
    }

    IdFile read_from(const std::function<char()> &func) {
        IdFile result;
        string qual, id;
        size_t i = 0;
        bool is_space = true;
        char c;
        int status = 0;
        do {
            c = func();
            if (c == EOF)
                break;
            if (isspace(c)) {
                if (!is_space) {
                    status = 0;
                    is_space = true;
                    result.add_id(Id{qual, id});
                    qual.clear();
                    id.clear();
                    if (c == '\n')
                        result.new_line();
                }
                continue;
            }
            is_space = false;
            switch (status) {
                case 0:
                    qual.push_back(c);
                    if (!is_qualifier(c))
                        status = 1;
                    break;
                case 1:
                    id.push_back(c);
                    if (is_qualifier(c))
                        throw std::runtime_error("Unexpected qualifier: " + std::to_string(c));
                    break;
                default:
                    throw std::runtime_error("Unknown status: " + std::to_string(status));
            }
        } while (true);
        return result;
    }

    IdFile read_file(const path &path) {
        ifstream ifs(path);
        if (!ifs.is_open())
            throw std::runtime_error("Cannot open file: " + path.string());
        return read_from([&ifs]() -> char {
            return static_cast<char>(ifs.get());
        });
    }

    IdFile read_string(string s) {
        s.push_back(EOF);
        size_t i = 0;
        return read_from([&s, &i]() -> char {
            return s[i++];
        });
    }

    Id generate_id() {
        static size_t count = 0;
        std::string buf;
        size_t c = count++;
        do {
            buf.push_back(static_cast<char>(c % 26 + 'a'));
            c /= 26;
        } while (c);
        return {"", buf};
    }
}
