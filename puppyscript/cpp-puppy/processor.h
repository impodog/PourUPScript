//
// Created by impodog on 7/30/2023.
//

#ifndef CPP_PUPPY_PROCESSOR_H
#define CPP_PUPPY_PROCESSOR_H

#include "const.h"

extern const std::unordered_map<Rule, int> default_rules;

bool starts_with(const std::string &s, const std::string &prefix);

bool ends_with(const std::string &s, const std::string &suffix);

bool is_spaces(const std::string &s);

using string_list = std::vector<std::string>;

string_list split(const std::string &s, char delimiter);

std::string join(string_list::const_iterator begin, string_list::const_iterator end, char delimiter);

std::string join(const string_list &list, char delimiter);

std::string left_strip(const std::string &s);

std::string right_strip(const std::string &s);

std::string strip(const std::string &s);

std::string indent_of(std::string line);

bool is_word(const std::string &s);

bool is_word_wider(const std::string &s);

std::string add_brackets(const std::string &s);

std::string add_with(const std::string &indent, const std::string &var);


std::string add_special(const std::string &name);

std::string word_base(const std::string &word);

bool is_debug_info(const std::string &line);

std::string read(const path &file);

void write(const path &file, const std::string &content);

class Globals {
public:
    path file_name;
    RuleTable rules = default_rules;

    std::string next_name(std::string identifier);

    explicit Globals(path file_name);
};

extern std::unique_ptr<Globals> globals;
extern RuleNames rule_names;

#define PROCESSOR(name) path name(path file, bool debug_mode, const std::string &output_name)

PROCESSOR(preprocess);

PROCESSOR(extract);

PROCESSOR(mid_process);

PROCESSOR(macro_rules);

PROCESSOR(syntactic);

PROCESSOR(type_format);

PROCESSOR(structure);

PROCESSOR(assignment);

PROCESSOR(brackets);

PROCESSOR(expr_optim);

PROCESSOR(shortcuts);

PROCESSOR(brackets_review);

#endif //CPP_PUPPY_PROCESSOR_H
