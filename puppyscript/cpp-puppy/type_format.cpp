//
// Created by impodog on 7/30/2023.
//
#include "processor.h"

std::regex RE_VAR_RAG(R"((\s*(attr|mth))\s+(.+)\s*)"),
        RE_METHOD("(\\s*)" FLAGS "(func|proc)\\s*(" WORD ")\\s*:\\s*"),
        RE_STATIC(R"((\s*)link\s+(.+))"),
        RE_CLASS("(\\s*)(enter\\s+)?class\\s+(" WORD ")(\\s+inh\\s+(.+))?\\s*:\\s*");

class TypeFormat {
    std::string content;
    std::unordered_map<std::string, std::string> string_created;

    std::string create_string(const std::string &value) {
        auto name = globals->next_name("CLS");
        string_created.insert({name, value});
        return name;
    }

    std::string add_variable_tags(const std::string &body, const std::string &indent) {
        string_list result;
        auto lines = split(body, '\n');
        for (auto line: lines) {
            auto cur_indent = indent_of(line);
            if (cur_indent.size() == indent.size()) {
                std::smatch results;
                if (std::regex_match(line, results, RE_VAR_RAG)) {
                    auto names = split(results[3].str(), ' ');
                    line = results[1].str();
                    for (const auto &name: names) {
                        line.push_back(' ');
                        line.append(create_string(name));
                    }
                }
            }
            result.push_back(line);
        }
        return join(result, '\n');
    }

    std::string add_method_tags(const std::string &body, const std::string &indent) {
        string_list result;
        bool is_begin_line = false;
        auto lines = split(body, '\n');
        for (auto line: lines) {
            auto cur_indent = indent_of(line);
            if (is_begin_line) {
                result.push_back(cur_indent + "param self");
                is_begin_line = false;
            } else if (cur_indent.size() == indent.size()) {
                std::smatch results;
                if (std::regex_match(line, results, RE_METHOD)) {
                    result.push_back(cur_indent + "mth " + create_string(results[4].str()));
                    is_begin_line = true;
                }
            }
            result.push_back(line);
        }
        return join(result, '\n');
    }

    std::string review_static(const std::string &body, const std::string &indent) {
        string_list result;
        auto lines = split(body, '\n');
        for (auto line: lines) {
            auto cur_indent = indent_of(line);
            if (cur_indent.size() == indent.size()) {
                std::smatch results;
                if (std::regex_match(line, results, RE_STATIC)) {
                    line = results[1].str() + results[2].str();
                }
            }
            result.push_back(line);
        }
        return join(result, '\n');
    }

    bool replace_class() {
        string_list result, class_body;
        std::string type_name, inherit, entering, outer_indent, inner_indent;
        bool changed = false;
        bool class_started = false, body_started = false;

        const auto &reset = [&type_name, &outer_indent, &inner_indent, &class_body, &inherit, &class_started, &body_started]() {
            type_name.clear();
            outer_indent.clear();
            inner_indent.clear();
            class_body.clear();
            inherit.clear();
            class_started = false;
            body_started = false;
        };

        auto lines = split(content, '\n');
        for (const auto &line: lines) {
            auto cur_indent = indent_of(line);
            if (type_name.empty()) {
                std::smatch results;
                if (std::regex_match(line, results, RE_CLASS)) {
                    changed = true;
                    outer_indent = results[1].str();
                    entering = results[2].str();
                    type_name = results[3].str();
                    result.push_back(outer_indent + (entering.empty() ? "type " : entering) + type_name + ":");
                    if (!results[4].str().empty()) {
                        string_list inherit_list;
                        auto inherit_names = results[5].str();
                        for (const auto &name: split(inherit_names, ' ')) {
                            if (!name.empty() && !is_spaces(name)) {
                                inherit_list.push_back(name[0] == '&' ? name : '&' + name);
                            }
                        }
                        inherit = join(inherit_list, ' ');
                    }
                } else {
                    result.push_back(line);
                }
            } else {
                if (inner_indent.empty()) {
                    inner_indent = cur_indent;
                    if (entering.empty()) {
                        result.push_back(inner_indent + "tid " + create_string(type_name));
                    }
                    if (!inherit.empty()) {
                        result.push_back(inner_indent + "inh " + inherit);
                    }
                }
                if (cur_indent.size() < inner_indent.size()) {
                    auto body = join(class_body, '\n');
                    body = add_variable_tags(body, inner_indent);
                    body = add_method_tags(body, inner_indent);
                    body = review_static(body, inner_indent);
                    result.push_back(body);
                    result.push_back(line);
                    reset();
                } else {
                    class_body.push_back(line);
                }
            }
        }
        content = join(result, '\n');
        return changed;
    }

    void replace_all_classes() {
        while (replace_class());
    }

    void write_into_constants(const std::string &output_name) {
        std::fstream out(output_name + CONST_SUFFIX, std::ios::app);
        for (auto &item: string_created) {
            out << item.first << " = " << item.second << "\n";
        }
        out.close();
    }

public:
    explicit TypeFormat(const path &file) {
        content = read(file);
    }

    path work(const std::string &output_name) {
        replace_all_classes();

        path output = output_name + ".type_format.puppy";
        write(output, content);
        write_into_constants(output_name);
        return output;
    }
};

PROCESSOR(type_format) {
    TypeFormat type_format(file);
    return type_format.work(output_name);
}