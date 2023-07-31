//
// Created by impodog on 7/30/2023.
//
#include "processor.h"

std::regex RE_INLINE_BLOCK(R"((.*?[^:]:)\s*([^:].*))");

class MidProcess {
    std::string content;

    void scan_inline_block() {
        string_list result;
        auto lines = split(content, '\n');
        for (const auto &line: lines) {
            std::smatch results;
            if (std::regex_match(line, results, RE_INLINE_BLOCK)) {
                std::string head = results[1].str();
                result.push_back(head);
                result.push_back("\t" + indent_of(head) + results[2].str());
            } else {
                result.push_back(line);
            }
        }
        content = join(result, '\n');
    }

    void scan_line_mixture() {
        string_list result;
        auto lines = split(content, '\n');
        for (const auto &line: lines) {
            auto statements = split(line, ';');
            if (statements.size() == 1) {
                result.push_back(line);
            } else {
                auto indent = indent_of(right_strip(statements[0]));
                result.push_back(indent + statements[0]);
                for (auto it = ++statements.begin(); it != statements.end(); it++) {
                    result.push_back(indent + *it);
                }
            }
        }
        content = join(result, '\n');
    }

    void fix_no_colon_lines() {
        if (!globals->rules.at(ALLOW_NO_COLON)) {
            return;
        }
        string_list result = {""};
        std::string prev_indent = "";
        auto lines = split(content, '\n');
        for (auto line: lines) {
            auto indent = indent_of(line);
            if (indent.size() > prev_indent.size() && result.back().back() != ':') {
                result.back() += ':';
            }
            result.push_back(line);
            prev_indent = indent;
        }
        content = join(++result.begin(), result.end(), '\n');
    }

public:
    explicit MidProcess(const path &file) {
        content = read(file);
    }

    path work(const std::string &output_name) {
        scan_inline_block();
        scan_line_mixture();
        fix_no_colon_lines();

        path output = output_name + ".mid_process.puppy";
        write(output, content);
        return output;
    }
};

PROCESSOR(mid_process) {
    auto mid_process = MidProcess(file);
    return mid_process.work(output_name);
}