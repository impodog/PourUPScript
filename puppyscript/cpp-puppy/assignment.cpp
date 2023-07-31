//
// Created by impodog on 7/31/2023.
//
#include "processor.h"

std::regex RE_ASSIGN(R"((\s*)(.+?)\s*=\s*(.+?)\s*)");

class Assignment {
    std::string content;

    void scan_assignments() {
        string_list result;
        auto lines = split(content, '\n');
        for (const auto &line: lines) {
            std::smatch results;
            if (std::regex_match(line, results, RE_ASSIGN)) {
                auto left = results[2].str();
                auto right = results[3].str();
                auto indent = indent_of(line);
                if (is_word_wider(left)) {
                    result.push_back(add_with(indent, right));
                    result.push_back(indent + "mov " + left);
                } else {
                    std::cerr << "Assigning to temporary expression \"" <<
                              left << "\" doesn't seem to have any effect."
                              << std::endl;
                }
            } else {
                result.push_back(line);
            }
        }
        content = join(result, '\n');
    }

public:
    explicit Assignment(const path &file) {
        content = read(file);
    }

    std::string work(const std::string &output_name) {
        scan_assignments();

        std::string output = output_name + ".assignment.puppy";
        write(output, content);
        return output;
    }
};

PROCESSOR(assignment) {
    Assignment assignment(file);
    return assignment.work(output_name);
}