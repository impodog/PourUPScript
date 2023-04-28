//
// Created by Dogs-Cute on 4/22/2023.
//

#ifndef POURUPSCRIPT_TOKEN_HPP
#define POURUPSCRIPT_TOKEN_HPP

#include "macros.h"

#define UNTIL_FN(name, ch) inline int name(char cur, char peek) {\
    return peek == ch && cur != '\\' ? 2 : 0;                    \
}
#define TAG "_t"
namespace PUPS {
    static std::stack<std::string> tags;

    struct TokenStruct {
        std::string token;
        bool _is_symbol = false;

        TokenStruct() = default;

        TokenStruct(std::string token, bool is_symbol) : token(std::move(token)), _is_symbol(is_symbol) {}
    };

    class Token {
        static const std::string *new_token(const char *token) {
            tags.emplace(token);
            return &tags.top();
        }

    protected:
        const std::string *token;
        bool _is_symbol = false;
    public:
        explicit Token() : token(nullptr) {}

        Token(TokenStruct &token_struct) : token(&token_struct.token),// NOLINT(google-explicit-constructor)
                                           _is_symbol(token_struct._is_symbol) {
        }

        explicit Token(std::string &token, bool is_symbol) noexcept: token(&token), _is_symbol(is_symbol) {}

        explicit Token(const char *token, bool is_symbol) noexcept: token(new_token(token)), _is_symbol(is_symbol) {}


        [[nodiscard]] const std::string &str() const noexcept {
            return *token;
        }

        [[nodiscard]] const std::string *ptr() const noexcept {
            return token;
        }

        [[nodiscard]] bool empty() const noexcept {
            return token->empty() || token->front() == ' ';
        }

        [[nodiscard]] bool eof() const noexcept {
            return token == nullptr;
        }

        void to_null() {
            token = nullptr;
        }

        FN_ALIAS(null, eof)

        [[nodiscard]] bool is_symbol() const noexcept {
            return _is_symbol;
        }

        [[nodiscard]] char front() const noexcept {
            return token->front();
        }

        [[nodiscard]] bool semicolon() const noexcept {
            return token->front() == ';';
        }

        bool operator==(const Token &cmp) const noexcept {
            return _is_symbol == cmp._is_symbol && *token == *cmp.token;
        }

        operator std::string() const { // NOLINT(google-explicit-constructor)
            return *token;
        }
    };

    const Token eofToken;

    std::ostream &operator<<(std::ostream &o, const Token &token) {
        return o << std::string(token);
    }

    static constexpr const int lower_diff = 'a' - 'A';
    static size_t tag_count = 0;


    Token next_tag() {
        tags.push(TAG + std::to_string(tag_count));
        return Token{tags.top(), false};
    }

    inline constexpr char to_lower(char c) noexcept {
        return ('A' <= c && c <= 'Z') ? (char) (c + lower_diff) : c;
    }

    inline constexpr bool is_alpha(char c) noexcept {
        return ('a' <= c && c <= 'z') || ('0' <= c && c <= '9') || c == '_';
    }

    inline constexpr bool is_empty(char c) noexcept {
        return c == ' ' || c == '\t' || c == '\n' || c == '\b';
    }

    UNTIL_FN(until_squote, '\'')

    UNTIL_FN(until_dquote, '\"')

    class TokenInput final {
        std::ifstream input;
        std::string _file;
        std::vector<TokenStruct> token_str;
        size_t line = 1, column = 0, index = 0;
        char cur = ' ', peek = ' ';

        friend std::ostream &operator<<(std::ostream &out, const TokenInput &input);

        char next_peek() {
            cur = peek;
            input.get(peek);
            peek = to_lower(peek);
            column++;
            if (column > USHRT_MAX) {
                throw std::runtime_error(
                        "Probable semicolon(;) loss at the end of file [" + std::to_string(line) + ":" +
                        std::to_string(column) + "]");
            }
            if (cur == '\n') {
                if (!input.eof()) {
                    line++;
                    column = 0;
                }
            }
            return peek;
        }

        bool next_token() {
            token_str.emplace_back();
            std::string &buf = token_str.back().token;
            bool &is_symbol = token_str.back()._is_symbol;
            while (true) {
                next_peek();
                if (is_alpha(cur))
                    buf.push_back(cur);
                else if (buf.empty() && !is_empty(cur)) {
                    buf.push_back(cur);
                    is_symbol = true;
                }
                if (cur == EOF || input.peek() == EOF) return true;
                if (!is_alpha(peek) || is_symbol)
                    return false;
            }
        }

        void mk_tokens() {
            while (!next_token()) {}
        }

    public:
        using NextUntil = int (*)(char cur, char peek);

        explicit TokenInput(const std::string &file) : input(file, std::ios::in), _file(file) {
            mk_tokens();
        }

        Token next() noexcept {
            TokenStruct *token;
            try {
                token = &token_str.at(index++);
            } catch (const std::out_of_range &) {
                return eofToken;
            }
            column += token->token.size();
            if (token->_is_symbol && token->token.front() == '\n') {
                line++;
                column = 0;
            }
            return *token;
        }

        const size_t &line_num() const noexcept {
            return line;
        }

        const size_t &column_num() const noexcept {
            return column;
        }

        const std::string &file() const noexcept {
            return _file;
        }
    };

    std::ostream &operator<<(std::ostream &out, const TokenInput &input) {
        for (auto &token: input.token_str)
            out << token.token;
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
}
#endif //POURUPSCRIPT_TOKEN_HPP
