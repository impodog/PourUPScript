//
// Created by Dogs-Cute on 4/22/2023.
//

#ifndef POURUPSCRIPT_TOKEN_HPP
#define POURUPSCRIPT_TOKEN_HPP

#include <utility>

#include "macros.h"

#define UNTIL_FN(name, ch) inline int name(char cur, char peek) {\
    return peek == ch && cur != '\\' ? 2 : 0;                    \
}
#define TAG "_t"
#define TMP "tmp"
namespace PUPS {
    static std::stack<std::string> tags;
    static constexpr const int lower_diff = 'a' - 'A';
    static size_t tag_count = 0;

    inline std::string next_file_s(const std::string &file) {
        static size_t file_count = 0;
        return file + "." + to_unique_str(file_count++) + "." TMP;
    }

    inline std::string wrap_quotes(const char *s) noexcept {
        return std::string("\"") + s + "\"";
    }

    inline constexpr bool is_alpha(char c) noexcept {
        return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || c == '_' || c == '.';
    }

    inline constexpr bool is_digit(char c) noexcept {
        return '0' <= c && c <= '9';
    }

    inline constexpr bool is_empty(char c) noexcept {
        return c == ' ' || c == '\t' || c == '\b';
    }

    inline constexpr bool is_true_empty(char c) noexcept {
        return c == ' ' || c == '\t' || c == '\b' || c == '\n';
    }

    inline std::queue<std::string> split_spaces(const std::string &s) {
        std::queue<std::string> result;
        bool is_space = true;
        for (auto c: s) {
            if (is_true_empty(c)) is_space = true;
            else {
                if (is_space) {
                    result.emplace();
                    is_space = false;
                }
                result.back().push_back(c);
            }
        }
        return result;
    }

    class Token {

        friend class TokenInput;

    protected:
        std::string token;
        bool _is_symbol = false, _is_long = false, _is_eof = false;

        friend Token make_eof();

    public:
        explicit Token() = default;

        explicit Token(std::string token, bool is_symbol = false, bool is_long = false) noexcept: token(
                std::move(token)),
                                                                                                  _is_symbol(
                                                                                                          is_symbol),
                                                                                                  _is_long(
                                                                                                          is_long) {}

        [[nodiscard]] const std::string &str() const noexcept {
            return token;
        }

        [[nodiscard]] std::string long_cut() const noexcept {
            return token.substr(1, token.size() - 2);
        }

        [[nodiscard]] std::string long_cut_no_space() const noexcept {
            std::string result;
            for (size_t i = 1, size = token.size() - 1; i < size; i++) {
                char c = token[i];
                if (!is_empty(c))
                    result.push_back(c);
            }
            return result;
        }

        // If is long, return the cut string, otherwise the whole str
        [[nodiscard]] std::string str_dependent() const noexcept {
            return is_long() ? long_cut() : str();
        }

        // If is long, return the cut string without spaces, otherwise the whole str
        [[nodiscard]] std::string str_dependent_no_space() const noexcept {
            return is_long() ? long_cut_no_space() : str();
        }

        [[nodiscard]] std::queue<std::string> split_by(char sep) const noexcept {
            std::queue<std::string> result;
            result.emplace();
            for (auto c: str_dependent()) {
                if (c == sep) result.emplace();
                else result.back().push_back(c);
            }
            return result;
        }

        [[nodiscard]] std::queue<std::string> split_by_no_space(char sep) const noexcept {
            std::queue<std::string> result;
            result.emplace();
            for (auto c: str_dependent_no_space()) {
                if (c == sep) result.emplace();
                else result.back().push_back(c);
            }
            return result;
        }

        [[nodiscard]] std::queue<std::string> split_stages() const noexcept {
            return split_by_no_space(':');
        }

        Token &append(char c) noexcept {
            token.push_back(c);
            return *this;
        }

        Token &append(const char *s) noexcept {
            token.append(s);
            return *this;
        }

        Token &append(const std::string &s) noexcept {
            token.append(s);
            return *this;
        }

        Token &convert_to_long() noexcept {
            token.push_back('}');
            token = '{' + token;
            _is_long = true;
            _is_symbol = _is_eof = false;
            return *this;
        }


        [[nodiscard]] const std::string *ptr() const noexcept {
            return &token;
        }

        [[nodiscard]] bool eof() const noexcept {
            return _is_eof;
        }

        [[nodiscard]] bool empty() const noexcept {
            return token.empty() || is_true_empty(token.front());
        }

        [[nodiscard]] bool reserved() const noexcept {
            return !token.empty() && token.front() == '_';
        }

        [[nodiscard]] bool null() const noexcept {
            return _is_eof || token.empty();
        }

        [[nodiscard]] bool is_symbol() const noexcept {
            return _is_symbol;
        }

        [[nodiscard]] bool is_symbol(char c) const noexcept {
            return _is_symbol && front() == c;
        }

        [[nodiscard]] bool is_long() const noexcept {
            return _is_long;
        }

        [[nodiscard]] char front() const noexcept {
            return token.front();
        }

        [[nodiscard]] bool semicolon() const noexcept {
            return token.front() == ';';
        }

        [[nodiscard]] bool colon() const noexcept {
            return token.front() == ':';
        }

        [[nodiscard]] bool linefeed() const noexcept {
            return token.front() == '\n';
        }

        [[nodiscard]] bool has_semicolon() const noexcept {
            for (auto iter = token.end() - 1; iter != token.begin(); iter--) {
                if (is_true_empty(*iter)) continue;
                else if (*iter == ';') return true;
                return false;
            }
            return token.front() == ';';
        }

        bool operator==(const Token &cmp) const noexcept {
            return _is_symbol == cmp._is_symbol && token == cmp.token;
        }

        bool operator==(const std::string &cmp) const noexcept {
            return token == cmp;
        }

        operator std::string() const { // NOLINT(google-explicit-constructor)
            return token;
        }
    };

    Token make_eof() {
        Token result;
        result._is_eof = true;
        return result;
    }

    const Token eofToken = make_eof();

    std::ostream &operator<<(std::ostream &o, const Token &token) {
        return o << std::string(token);
    }

    inline Token builtin_marked(const std::string &s) {
        return Token{builtin_mark + s};
    }

    inline Token next_tag() {
        tags.push(TAG + to_unique_str(tag_count++));
        return Token{tags.top()};
    }

    template<typename ArrT>
    inline std::string join_tokens(const ArrT &tokens) {
        std::string result;
        for (auto &token: tokens)
            result.append(token);
        return result;
    }

    inline std::string join_tokens(std::queue<Token> &tokens) {
        std::string result;
        while (!tokens.empty()) {
            result.append(tokens.front());
            tokens.pop();
        }
        return result;
    }

    UNTIL_FN(until_squote, '\'')

    UNTIL_FN(until_dquote, '\"')

    class TokenInput final {
        std::ifstream input;
        fpath _file;
        std::string _cur_line;
        std::vector<Token> tokens;
        size_t line = 1, index = 0;
        char cur = ' ', peek = ' ';

        friend std::ostream &operator<<(std::ostream &out, const TokenInput &input);

        char next_peek() {
            cur = peek;
            input.get(peek);
            return peek;
        }

        bool next_token() {
            tokens.emplace_back();
            std::string &buf = tokens.back().token;
            bool &is_symbol = tokens.back()._is_symbol, &is_long = tokens.back()._is_long;
            size_t braced_mode = 0;
            while (true) {
                next_peek();
                if (cur == '{') {
                    is_long = true;
                    braced_mode++;
                } else if (cur == '}') {
                    braced_mode--;
                    if (!braced_mode)
                        buf.push_back(cur);
                }
                if (cur == EOF || input.peek() == EOF) return true;
                if (braced_mode)
                    buf.push_back(cur);
                else {
                    if (is_alpha(cur))
                        buf.push_back(cur);
                    else if (buf.empty() && !is_empty(cur)) {
                        buf.push_back(cur);
                        is_symbol = true;
                    }
                    if (!is_alpha(peek) || is_symbol)
                        return false;
                }
            }
        }

        void mk_tokens() {
            while (!next_token()) {}
        }

    public:
        using NextUntil = int (*)(char cur, char peek);

        explicit TokenInput(const fpath &file) : input(file, std::ios::in), _file(file) {
            mk_tokens();
        }

        Token next() noexcept {
            Token token;
            try {
                token = tokens.at(index++);
            } catch (const std::out_of_range &) {
                return eofToken;
            }
            _cur_line.append(token.token).push_back(' ');
            if (token._is_symbol && token.token.front() == '\n') {
                line++;
                _cur_line.clear();
            } else if (token._is_long) {
                line += std::count(token.token.begin(), token.token.end(), '\n');
            }
            return token;
        }

        const size_t &line_num() const noexcept {
            return line;
        }

        const std::string &cur_line() const noexcept {
            return _cur_line;
        }

        std::string file() const noexcept {
            return _file.string();
        }
    };

    std::ostream &operator<<(std::ostream &out, const TokenInput &input) {
        for (auto &token: input.tokens)
            out << token.str();
        return out;
    }
}
namespace std {
    template<>
    struct hash<PUPS::Token> {
        size_t operator()(const PUPS::Token &token) const noexcept {
            //if (token.eof())
            //    throw std::runtime_error("EOF PUPS token is not hashable");
            return std::hash<std::string>()(token.str());
        }
    };

    template<>
    struct less<PUPS::Token> {
        bool operator()(const PUPS::Token &x, const PUPS::Token &y) const { return x.str() < y.str(); }
    };
}
#endif //POURUPSCRIPT_TOKEN_HPP
