//
// Created by impodog on 7/30/2023.
//
#include "processor.h"

std::regex RE_ELSE(R"((\s*)else\s*:)"),
        RE_IF(R"((\s*)((if|elif)(_not)?)\s+(.+):)"),
        RE_FOR_BASIC(R"((\s*)for\s+(.+?)\s*,\s*(.+?)\s*,\s*(.+?)\s*:\s*)"),
        RE_FOR_RANGE("(\\s*)for\\s+(" WORD ")\\s+in\\s+(.+?):\\s*"),
        RE_WHILE(R"((\s*)while\s+(.+):)"),
        RE_MATCH_CASE(R"((\s*)(match|case)\s+(.+):\s*)");

class Structure {
public:
    std::string content;

    explicit Structure(const path &file) {
        content = read(file);
    }

    void scan_if() {
        string_list result;
        auto lines = split(content, '\n');
        for (const auto &line: lines) {
            std::smatch results;
            if (std::regex_match(line, results, RE_ELSE)) {
                result.push_back(results[1].str() + WITH_STMT " true");
                result.push_back(results[1].str() + "elif:");
            } else if (std::regex_match(line, results, RE_IF)) {
                result.push_back(add_with(results[1].str(), results[5].str()));
                result.push_back(results[1].str() + results[2].str() + ":");
            } else {
                result.push_back(line);
            }
        }
        content = join(result, '\n');
    }

    bool scan_for() {
        string_list result;
        std::string *for_name = nullptr;
        std::string outer_indent, inner_indent;
        bool changed = false;

        auto lines = split(content, '\n');
        for (const auto &line: lines) {
            if (for_name) {
                auto cur_indent = indent_of(line);
                if (inner_indent.empty()) {
                    inner_indent = cur_indent;
                } else {
                    if (cur_indent.size() < inner_indent.size()) {
                        result.push_back(inner_indent + for_name[2]);
                        if (!for_name[3].empty()) {
                            result.push_back(outer_indent + "~" + for_name[3]);
                        }
                        delete[] for_name;
                        for_name = nullptr;
                        outer_indent.clear();
                        inner_indent.clear();
                    }
                }
                result.push_back(line);
            } else {
                std::smatch results;
                if (std::regex_match(line, results, RE_FOR_BASIC)) {
                    outer_indent = results[1].str();
                    for_name = new std::string[4];
                    for_name[0] = results[2].str();
                    for_name[1] = results[3].str();
                    for_name[2] = results[4].str();
                    for_name[3] = "";
                } else if (std::regex_match(line, results, RE_FOR_RANGE)) {
                    outer_indent = results[1].str();
                    auto tmp_name = globals->next_name("FOR");
                    result.push_back(outer_indent + tmp_name + " = " + results[3].str());
                    for_name = new std::string[4];
                    for_name[0] = results[2].str() + " = call " + tmp_name + ".beg";
                    for_name[1] = "call " + tmp_name + ".cond";
                    for_name[2] = results[2].str() + " = call " + tmp_name + ".next";
                    for_name[3] = tmp_name;
                }
                if (for_name) {
                    changed = true;
                    result.push_back(outer_indent + for_name[0]);
                    result.push_back(outer_indent + "while " + for_name[1] + ":");
                    continue;
                }
                result.push_back(line);
            }
        }
        content = join(result, '\n');
        return changed;
    }

    void scan_all_for() {
        while (scan_for());
    }

    bool scan_while() {
        string_list result;
        std::string while_name;
        bool end_scanning = false;
        std::list<std::string> stack;
        std::string outer_indent, inner_indent;

        auto lines = split(content, '\n');
        for (const auto &line: lines) {
            if (while_name.empty()) {
                std::smatch results;
                if (std::regex_match(line, results, RE_WHILE)) {
                    while_name = results[2].str();
                    outer_indent = results[1].str();
                    stack = {outer_indent + "loop:"};
                } else {
                    result.push_back(line);
                }
            } else if (end_scanning) {
                result.push_back(line);
            } else {
                stack.push_back(line);
                if (inner_indent.empty()) {
                    inner_indent = indent_of(line);
                } else if (indent_of(line).size() < inner_indent.size()) {
                    stack.pop_back();
                    stack.front() = outer_indent + "if:" + strip(stack.front());
                    stack.insert(stack.begin(), add_with(outer_indent, while_name));
                    stack.push_back(inner_indent + "break " + add_brackets(while_name));
                    result.insert(result.end(), stack.begin(), stack.end());
                    result.push_back(line);
                    end_scanning = true;
                }
            }
        }
        content = join(result, '\n');
        return !while_name.empty();
    }

    void scan_all_while() {
        while (scan_while());
    }

    void scan_match_case() {
        string_list result;
        auto lines = split(content, '\n');
        for (const auto &line: lines) {
            std::smatch results;
            if (std::regex_match(line, results, RE_MATCH_CASE)) {
                result.push_back(add_with(results[1].str(), results[3].str()));
                result.push_back(results[1].str() + results[2].str() + ":");
            } else {
                result.push_back(line);
            }
        }
        content = join(result, '\n');
    }

    std::string work(const std::string &output_name) {
        scan_if();
        scan_all_for();
        scan_all_while();
        scan_match_case();

        std::string output = output_name + ".struct.puppy";
        write(output, content);
        return output;
    }
};

PROCESSOR(structure) {
    Structure structure(file);
    return structure.work(output_name);
}