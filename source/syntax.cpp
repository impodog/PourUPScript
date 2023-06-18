//
// Created by Dogs-Cute on 5/27/2023.
//

#include "syntax.h"
//#include "debug/record.h"

#include <utility>

namespace pups::library {

    IdPtr empty_id = std::make_shared<Id>();
    IdFactor empty_factor{empty_id};
    Id id_args{"", "args"};

    bool is_qualifier(char c) {
        return c == '$' || c == '&' || c == '%';
    }

    Id::Id(std::string qual, std::string id) : m_qual(std::move(qual)), m_id(std::move(id)) {

    }

    Id::Id(const std::string &qual_id) {
        int status = 0;
        bool is_dotted = false;
        for (auto c: qual_id) {
            switch (status) {
                case 0:
                    if (!is_qualifier(c)) {
                        m_id.push_back(c);
                        status = 1;
                    } else
                        m_qual.push_back(c);
                    break;
                case 1:
                    m_id.push_back(c);
                    if (is_qualifier(c)) {
                        if (!is_dotted)
                            throw std::runtime_error(
                                    "Unexpected qualifier: " + std::to_string(c) + " in id \"" + qual_id + "\"");
                    } else
                        is_dotted = false;
                    if (c == '.')
                        is_dotted = true;
                    break;
                default:
                    throw std::runtime_error("Unknown status: " + std::to_string(status));
            }
        }
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

    std::queue<std::string> Id::split_by(char sep) const {
        std::queue<std::string> result;
        result.emplace();
        for (auto c: str())
            if (c == sep) result.emplace();
            else result.back().push_back(c);
        return result;
    }

    bool Id::empty() const noexcept {
        return m_id.empty();
    }

    bool Id::lf() const noexcept {
        return m_id.front() == '\n';
    }

    bool Id::lbrace() const noexcept {
        return m_id.front() == '{';
    }

    bool Id::rbrace() const noexcept {
        return m_id.front() == '}';
    }

    bool Id::operator==(const Id &rhs) const {
        return m_id == rhs.m_id;
    }

    bool Id::operator==(const std::string &rhs) const {
        return m_id == rhs;
    }

    size_t Id::hash::operator()(const Id &id) const {
        return std::hash<std::string>()(id.m_id);
    }

    IdFactor::IdFactor(IdPtr id) : m_type(t_id), m_id{std::move(id)} {

    }

    IdFactor::IdFactor(IdFilePtr idFile) : m_type(t_idFile), m_idFile{std::move(idFile)} {

    }

    bool IdFactor::is_id() const noexcept {
        return m_type == t_id;
    }

    bool IdFactor::empty() const noexcept {
        return is_id() && m_id->empty();
    }

    const IdPtr &IdFactor::id() const noexcept {
        return m_id;
    }

    const IdFilePtr &IdFactor::idFile() const noexcept {
        return m_idFile;
    }

    std::string IdFactor::str() const noexcept {
        switch (m_type) {
            case t_id:
                return m_id->str();
            case t_idFile:
                return m_idFile->str();
        }
        return "[ERROR]";
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

    const IdFactor &IdFile::get_id() const {
        if (m_cursor.first == m_file.size())
            return empty_factor;
        try {
            return m_file.at(m_cursor.first).at(m_cursor.second);
        } catch (const std::out_of_range &) {
            return empty_factor;
        }
    }

    const IdFactor &IdFile::next_id(bool &is_new_line) {
        is_new_line = next_cursor();
        return get_id();
    }

    void IdFile::add_id(const IdPtr &id) {
        m_file.back().emplace_back(id);
    }

    void IdFile::add_idFile(const IdFilePtr &idFile) {
        m_file.back().emplace_back(idFile);
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

    void IdFile::restart() noexcept {
        m_cursor = {0, 0};
    }

    IdFile::Line IdFile::all() const noexcept {
        Line result;
        for (auto &line: m_file) {
            for (auto &id: line)
                result.push_back(id);
        }
        return result;
    }

    std::string IdFile::str() const noexcept {
        std::string result;
        result.append("{\n");
        for (auto &line: m_file) {
            for (auto &id: line) {
                result.append(id.str()).push_back(' ');
            }
            result.append("\n");
        }
        result.append("}");
        return result;
    }

    IdFile read_from(const SyntaxFunc &func, const SyntaxFunc &peek) {
        IdFile result;
        std::string id;
        bool is_space = true;
        char c;
        int status = 0;
        const auto add_id = [&result, &id]() {
            if (!id.empty())
                result.add_id(std::make_shared<Id>(id));
            id.clear();
        };
        do {
            c = func();
            if (c == EOF)
                break;
            else if (c == ':') {
                status = 0;
                is_space = true;
                add_id();
                auto tmp = read_block(func, peek);
                result.add_idFile(std::make_shared<IdFile>(tmp));
                result.new_line(); // There is always new line after blocks
                continue;
            } else if (isspace(c)) {
                if (!is_space) {
                    status = 0;
                    is_space = false;

                    if (!id.empty())
                        add_id();

                    if (c == '\n')
                        result.new_line();
                }
                continue;
            }
            is_space = false;
            id.push_back(c);
        } while (true);
        if (!id.empty())
            result.add_id(std::make_shared<Id>(id));
        return result;
    }

    IdFile read_file(const path &path) {
        std::ifstream ifs(path);
        if (!ifs.is_open())
            throw std::runtime_error("Cannot open file: " + path.string());
        return read_from([&ifs]() -> char {
            return static_cast<char>(ifs.get());
        }, [&ifs]() -> char {
            return static_cast<char>(ifs.peek());
        });
    }

    IdFile read_string(std::string s) {
        s.push_back(EOF);
        size_t i = 0;
        return read_from([&s, &i]() -> char {
            return s[i++];
        }, [&s, &i]() -> char {
            if (s[i] == EOF)
                return EOF;
            return s[i + 1];
        });
    }

    Id generate_id() {
        static size_t count = 0;
        std::string buf = RESERVED;
        size_t c = count++;
        do {
            buf.push_back(static_cast<char>(c % 26 + 'a'));
            c /= 26;
        } while (c);
        return Id{"", buf};
    }

    IdFile read_block(const SyntaxFunc &func, const SyntaxFunc &peek) {
        char prev = EOF;
        char c = peek();
        bool depth_count = false;
        size_t depth = 0, depth_c = -1;
        auto block = read_from([&c, &prev, &depth_count, &func, &peek, &depth, &depth_c]() -> char {
            if (depth_count) {
                depth_count = false;
                if (isspace(c)) {
                    depth_c++;
                    depth_count = true;
                } else if (depth) {
                    if (depth_c < depth)
                        return EOF;
                } else {
                    depth = depth_c;
                }
            }
            if (c == '\n') {
                depth_count = true;
                depth_c = 0;
            }
            prev = func();
            c = peek(); // The first peek is the second char after colon
            return prev;
        }, peek);
        return block;
    }

    std::string to_string(const void *ptr) {
        std::ostringstream out;
        out << std::hex << ptr;
        return out.str();
    }
}
