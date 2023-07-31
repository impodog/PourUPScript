//
// Created by impodog on 7/30/2023.
//

#include "processor.h"

const std::unordered_map<Rule, int> default_rules = {
        {MODULE,         1},
        {ALLOW_NO_COLON, 0}
};

std::unique_ptr<Globals> globals;
RuleNames rule_names = {
        {"module",         MODULE},
        {"allow-no-colon", ALLOW_NO_COLON}
};

bool starts_with(const std::string &s, const std::string &prefix) {
    const size_t len = prefix.size();
    if (len > s.size()) {
        return false;
    }
    for (size_t pos = 0; pos < len; pos++) {
        if (s[pos] != prefix[pos]) {
            return false;
        }
    }
    return true;
}

bool ends_with(const std::string &s, const std::string &suffix) {
    const size_t len = suffix.size(), s_len = s.size();
    if (len > s.size()) {
        return false;
    }
    const size_t begin = s_len - len;
    for (size_t pos = 0; pos < len; pos++) {
        if (s[begin + pos] != suffix[pos]) {
            return false;
        }
    }
    return true;
}

bool is_spaces(const std::string &s) {
    for (char c: s) {
        if (!isspace(c)) {
            return false;
        }
    }
    return true;
}

string_list split(const std::string &s, const char delimiter) {
    string_list result;
    std::string temp;
    for (char c: s) {
        if (c == delimiter) {
            if (!temp.empty()) {
                result.push_back(temp);
                temp.clear();
            }
        } else {
            temp.push_back(c);
        }
    }
    if (!temp.empty()) {
        result.push_back(temp);
    }
    return result;
}

std::string join(string_list::const_iterator begin, string_list::const_iterator end, char delimiter) {
    if (begin == end)
        return "";
    std::string result = *begin;
    for (auto it = ++begin; it != end; it++) {
        result.push_back(delimiter);
        result += *it;
    }
    return result;
}

std::string join(const string_list &list, char delimiter) {
    return join(list.begin(), list.end(), delimiter);
}

std::string left_strip(const std::string &s) {
    size_t pos = 0;
    while (pos < s.size() && isspace(s[pos])) {
        pos++;
    }
    return s.substr(pos);
}

std::string right_strip(const std::string &s) {
    size_t pos = s.size();
    while (pos > 0 && isspace(s[pos - 1])) {
        pos--;
    }
    return s.substr(0, pos);
}

std::string strip(const std::string &s) {
    return right_strip(left_strip(s));
}

std::string indent_of(std::string line) {
    std::string result;
    for (char c: line) {
        if (isspace(c)) {
            result.push_back(c);
        } else {
            return result;
        }
    }
    return "";
}

bool is_word(const std::string &s) {
    return std::regex_match(s, RE_WORD);
}

bool is_word_wider(const std::string &s) {
    size_t index = s.rfind(')');
    if (index == -1) {
        return is_word(s);
    } else {
        return is_word(s.substr(index + 1));
    }
}

std::string add_brackets(const std::string &s) {
    if (is_word_wider(s)) {
        return s;
    } else {
        return "(" + s + ")";
    }
}

std::string add_with(const std::string &indent, const std::string &var) {
    if (is_word_wider(var)) {
        return indent + WITH_STMT " " + var;
    } else {
        return indent + var;
    }
}

std::string add_special(const std::string &name) {
    return SPECIAL_PREFIX + name;
}

std::string word_base(const std::string &word) {
    return word.substr(std::max(word.rfind('.'), word.rfind('&')) + 1);
}

bool is_debug_info(const std::string &line) {
    return starts_with(line, DEBUG_INFO_STMT);
}

std::string read(const path &file) {
    std::ifstream in(file);
    if (!in.is_open()) {
        throw std::runtime_error("cannot open file: " + file.string());
    }
    std::string result;
    std::string line;
    while (std::getline(in, line)) {
        result.append(line).push_back('\n');
    }
    in.close();
    return result;
}

void write(const path &file, const std::string &content) {
    std::ofstream out(file);
    out << content;
    out.close();
}

std::string Globals::next_name(std::string identifier) {
    static size_t count = 0;
    size_t value = ++count;
    identifier.push_back('_');
    while (value) {
        identifier.push_back(value % 26 + 'A');
        value /= 26;
    }
    if (rules.at(MODULE)) {
        return "_pp_" + file_name.stem().string() + "_" + identifier;
    } else {
        return "_pp_" + identifier;
    }
}

Globals::Globals(path file_name) : file_name(std::move(file_name)) {}
