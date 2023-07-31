//
// Created by impodog on 7/30/2023.
//
#include "processor.h"

std::regex RE_RETURN(R"((\s*)ret\s+(.+?)\s*)"),
        RE_BREAK(R"((\s*)break\s*)"),
        RE_CONTINUE(R"((\s*)conti\s*)"),
        RE_UNMAP_IMPORT(R"((\s*)(use|unmap)\s+import\s+(.+))"),
        RE_IMPORT_AS(R"((\s*)import\s+()" WORD R"()\s+as\s+()" WORD R"())"),
        RE_USE_AS(R"((\s*)use\s+()" WORD R"()\s+as\s+()" WORD R"())"),
        RE_SPECIAL_FUNCTION("(\\s*)spec\\s+" FLAGS "(func|proc)\\s+(" WORD ")\\s*:\\s*"),
        RE_LINK_TO("(\\s*)link\\s+(" WORD ")\\s+to\\s+(" WORD ")\\s*"),
        RE_EXTERN(R"((\s*)pub(_all)?\s+(.*))"),
        RE_PUB_ASSIGN("(\\s*)pub\\s+(" WORD ")\\s*=\\s*(.+)");

class Syntactic {
    std::string content;

    void scan_returns() {
        string_list result;
        auto lines = split(content, '\n');
        for (auto &line: lines) {
            std::smatch results;
            if (std::regex_match(line, results, RE_RETURN)) {
                result.push_back(add_with(indent_of(line), results[2].str()));
                result.push_back(indent_of(line) + "ret");
            } else {
                result.push_back(line);
            }
        }
        content = join(result, '\n');
    }

    void scan_break_and_continue() {
        string_list result;
        auto lines = split(content, '\n');
        for (auto &line: lines) {
            if (std::regex_match(line, RE_BREAK)) {
                result.push_back(indent_of(line) + "break false");
            } else if (std::regex_match(line, RE_CONTINUE)) {
                result.push_back(indent_of(line) + "break true");
            } else {
                result.push_back(line);
            }
        }
        content = join(result, '\n');
    }

    void scan_unmap_import() {
        string_list result;
        auto lines = split(content, '\n');
        for (auto &line: lines) {
            std::smatch results;
            if (std::regex_match(line, results, RE_UNMAP_IMPORT)) {
                result.push_back(results[1].str() + "import " + results[3].str());
                result.push_back(results[1].str() + "unmap");
            } else {
                result.push_back(line);
            }
        }
        content = join(result, '\n');
    }

    void scan_import_as() {
        string_list result;
        auto lines = split(content, '\n');
        for (auto &line: lines) {
            std::smatch results;
            if (std::regex_match(line, results, RE_IMPORT_AS)) {
                result.push_back(results[1].str() + "import " + results[2].str());
                result.push_back(results[1].str() + "mov " + results[3].str());
            } else {
                result.push_back(line);
            }
        }
        content = join(result, '\n');
    }

    void scan_use_as() {
        string_list result;
        auto lines = split(content, '\n');
        for (auto &line: lines) {
            std::smatch results;
            if (std::regex_match(line, results, RE_USE_AS)) {
                result.push_back(add_with(results[1].str(), results[2].str()));
                if (results[4].matched) {
                    result.push_back(results[1].str() + "mov " + results[3].str());
                } else {
                    result.push_back(add_with(results[1].str(), results[2].str()));
                    result.push_back(results[1].str() + "mov " + word_base(results[2].str()));
                }
            } else {
                result.push_back(line);
            }
        }
        content = join(result, '\n');
    }

    void scan_special_function() {
        string_list result;
        auto lines = split(content, '\n');
        for (auto &line: lines) {
            std::smatch results;
            if (std::regex_match(line, results, RE_SPECIAL_FUNCTION)) {
                result.push_back(results[1].str() + results[3].str()
                                 + " " + add_special(results[4].str()) + ":");
            } else {
                result.push_back(line);
            }
        }
        content = join(result, '\n');
    }

    void scan_link_to() {
        string_list result;
        auto lines = split(content, '\n');
        for (auto &line: lines) {
            std::smatch results;
            if (std::regex_match(line, results, RE_LINK_TO)) {
                result.push_back(results[1].str() +
                                 results[3].str() + "." + word_base(results[2].str())
                                 + " = " + results[2].str());
            } else {
                result.push_back(line);
            }
        }
        content = join(result, '\n');
    }

    bool scan_extern() {
        string_list result;
        size_t indent = -1;
        string_list externals;
        bool succeeded = false;
        bool pub_all = false;
        auto lines = split(content, '\n');
        for (auto line: lines) {
            if (indent != -1) {
                auto cur_indent = indent_of(line).size();
                if (cur_indent < indent) {
                    indent = -1;
                    externals.clear();
                    pub_all = false;
                } else if (pub_all || cur_indent == indent) {
                    for (auto &name: externals) {
                        line = std::regex_replace(line, std::regex("([^\\w.&])" + name + "(\\b)"),
                                                  "$1&" + name + "$2");
                    }
                }
                result.push_back(line);
            } else {
                std::smatch results;
                if (std::regex_match(line, results, RE_EXTERN)) {
                    indent = results[1].str().size();
                    if (results[2].matched) {
                        pub_all = true;
                    }
                    externals = split(results[3].str(), ' ');
                    succeeded = true;
                } else {
                    result.push_back(line);
                }
            }
        }
        content = join(result, '\n');
        return succeeded;
    }

    void scan_all_extern() {
        content = std::regex_replace(content, RE_PUB_ASSIGN, R"($1pub $2$1$2 = $3)");
        while (scan_extern());
    }

public:
    explicit Syntactic(const path &path) {
        content = read(path);
    }

    path work(const std::string &output_name) {
        scan_returns();
        scan_break_and_continue();
        scan_unmap_import();
        scan_import_as();
        scan_use_as();
        scan_special_function();
        scan_link_to();
        scan_all_extern();

        path output = output_name + ".syntactic.puppy";
        write(output, content);
        return output;
    }

};

PROCESSOR(syntactic) {
    Syntactic syntactic(file);
    return syntactic.work(output_name);
}