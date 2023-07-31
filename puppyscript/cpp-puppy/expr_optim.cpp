//
// Created by impodog on 7/31/2023.
//
#include "processor.h"

std::regex RE_ARRAY_INDEXES("\\b(" WORD ")\\[");

class ExprOptim {
    std::string content;

    void scan_array_indexes(std::string &s) {
        std::smatch results;
        while (std::regex_search(s, results, RE_ARRAY_INDEXES)) {
            auto arr = results[1].str();
            size_t start = results.position(1) + arr.size();
            size_t depth = 0, pos = start;
            do {
                switch (s[pos]) {
                    case '[':
                        depth++;
                        break;
                    case ']':
                        depth--;
                        break;
                    default:
                        break;
                }
                pos++;
            } while (depth);
            auto subscript = s.substr(start + 1, pos - start - 2);
            scan_array_indexes(subscript);

            std::string replacement = "(";
            replacement.append(arr).append(".at_ref ")
                    .append(subscript).append(")").append(".data");
            s.replace(results.position(0), pos - results.position(0), replacement);
        }
    }

    void scan_all_array_indexes() {
        auto lines = split(content, '\n');
        for (auto &line: lines) {
            scan_array_indexes(line);
        }
        content = join(lines, '\n');
    }

public:
    ExprOptim(const path &file) {
        content = read(file);
    }

    std::string work(const std::string &output_name) {
        scan_all_array_indexes();

        std::string output = output_name + ".expr_optim.puppy";
        write(output, content);
        return output;
    }
};

PROCESSOR(expr_optim) {
    ExprOptim expr_optim(file);
    return expr_optim.work(output_name);
}