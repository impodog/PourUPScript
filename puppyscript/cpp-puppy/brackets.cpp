//
// Created by impodog on 7/31/2023.
//
#include "processor.h"

std::regex RE_REDUNDANT(R"((\s*)\((.+)\)\s*)"),
        RE_HAS_ATTACHMENT(".+" WORD);

class Brackets {
    std::string content;
    std::stack<std::string> temporary;
    string_list output_list;

    void remove_redundant() {
        string_list result;
        string_list lines = split(content, '\n');
        for (const auto &line: lines) {
            std::smatch results;
            if (std::regex_match(line, results, RE_REDUNDANT)) {
                result.push_back(add_with(results[1].str(), results[2].str()));
            } else {
                result.push_back(line);
            }
        }
        content = join(result, '\n');
    }

    void push_line(const std::string &line) {
        if (!line.empty()) {
            output_list.push_back(line);
        }
    }

    void reset() {
        while (!temporary.empty()) {
            temporary.pop();
        }
    }

    void scan_bracket(std::string s, const std::string &indent, bool is_top=true) {
        s = strip(s);

        size_t depth = 0, start = NPOS;
        size_t size = s.size();

        for (size_t i = 0; i < size; i++) {
            switch (s[i]) {
                case '(':
                    depth++;
                    if (start == NPOS) {
                        start = i;
                    }
                    break;
                case ')':
                    depth--;
                    break;
                default:
                    break;
            }
            if (start != NPOS && depth == 0) {
                if (start && std::regex_match(s.substr(0, start), RE_HAS_ATTACHMENT)) {
                    start = NPOS;
                } else {
                    auto part = s.substr(start + 1, i - start - 1);
                    auto name = globals->next_name("FAC");
                    temporary.push(name);
                    scan_bracket(part, indent, false);
                    push_line(indent + "mov " + name);

                    auto prefix = s.substr(0, start),
                            suffix = s.substr(i + 1);
                    part = prefix;
                    part.append(name).append(suffix);

                    if (!is_spaces(prefix) || !is_spaces(suffix)) {
                        scan_bracket(part, indent, false);
                    }
                    return;
                }
            }
        }

        if (is_top)
            push_line(indent + s);
        else
            push_line(add_with(indent, s));
    }

    void scan_brackets() {
        string_list lines = split(content, '\n');
        for (const auto &line: lines) {
            auto indent = indent_of(line);
            reset();
            scan_bracket(line, indent);
            while (!temporary.empty()) {
                push_line(indent + "~" + temporary.top());
                temporary.pop();
            }
        }
    }

public:
    explicit Brackets(const path &file) {
        content = read(file);
    }

    std::string work(const std::string &output_name, bool is_review) {
        remove_redundant();
        scan_brackets();

        if (is_review) {
            std::string output = output_name + ".brackets_review.puppy";
            write(output, join(output_list, '\n'));
            return output;
        }
        else {
            std::string output = output_name + ".brackets.puppy";
            write(output, join(output_list, '\n'));
            return output;
        }
    }
};

PROCESSOR(brackets) {
    Brackets brackets(file);
    return brackets.work(output_name, false);
}

PROCESSOR(brackets_review) {
    Brackets brackets(file);
    return brackets.work(output_name, true);
}
