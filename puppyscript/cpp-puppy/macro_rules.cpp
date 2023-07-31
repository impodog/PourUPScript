//
// Created by impodog on 7/30/2023.
//
#include "processor.h"

std::regex RE_MACRO_DEF(R"(\s*macro\s+(\w+(!?))\s*:\s*)"),
        RE_FORM_DEF(R"(\s*form(\s+(\w[\w\s.]*))?\s*:\s*)");

using FormArgs = std::unordered_map<std::string, std::string>;

class Form {
public:
    string_list args, body;

    Form(const std::string &args, const std::string &body) {
        this->args = split(args, ' ');
        auto base_indent = indent_of(body);
        auto lines = split(body, '\n');
        for (const auto &line: lines) {
            this->body.push_back(line.substr(base_indent.size()));
        }
    }

    string_list replace_with(const FormArgs &assign, const std::string &base_indent) const {
        string_list result;
        bool is_first = true;
        for (auto line: body) {
            for (const auto &repl: assign) {
                while (true) {
                    size_t pos = line.find(repl.first);
                    if (pos == std::string::npos) {
                        break;
                    }
                    size_t end = pos + repl.first.size();
                    if (pos > 0 && line[pos - 1] == '#') {
                        pos--;
                    }
                    if (end < line.size() && line[end] == '#') {
                        end++;
                    }
                    line = line.substr(0, pos) + repl.second + line.substr(end);
                }
            }
            if (is_first) {
                result.push_back(line);
                is_first = false;
            } else {
                result.push_back(base_indent + line);
            }
        }
        return result;
    }
};

class Macro {
public:
    std::string name;
    std::vector<Form> forms;

    Macro(std::string name, std::vector<Form> forms) :
            name(std::move(name)), forms(std::move(forms)) {

    }

    std::string try_replace_with(const std::string &args_str, const std::string &base_indent) const {
        auto args = split(args_str, ' ');
        size_t arg_len = args.size();
        for (const auto &form: forms) {
            FormArgs assign;
            size_t form_len = form.args.size();
            if (form_len > arg_len) {
                continue;
            }
            if (form_len == 0) {
                if (arg_len == 0) {
                    // intended to do nothing
                } else {
                    continue;
                }
            } else if (ends_with(form.args.back(), ELLIPSIS)) {
                for (size_t i = 0; i < form_len - 1; i++) {
                    assign.insert({form.args[i], args[i]});
                }
                const auto &e_name = form.args.back();
                assign[e_name.substr(0, e_name.size() - ELLIPSIS_SIZE)] =
                        join(args.begin() + form_len - 1, args.end(), ' ');
            } else if (ends_with(form.args.front(), ELLIPSIS)) {
                for (size_t i = 1; i < form_len; i++) {
                    assign.insert({form.args[i], args[arg_len - form_len + i]});
                }
                const auto &e_name = form.args.front();
                assign[e_name.substr(0, e_name.size() - ELLIPSIS_SIZE)] =
                        join(args.begin(), args.end() - form_len + 1, ' ');
            } else if (arg_len == form_len) {
                for (size_t i = 0; i < form_len; i++) {
                    assign.insert({form.args[i], args[i]});
                }
            }

            if (assign.size() == form_len) {
                return join(form.replace_with(assign, base_indent), '\n');
            }
        }
        throw std::runtime_error("");
    }
};

class MacroRules {
    std::string content;
    std::vector<Macro> macros;
    bool changed;

    std::string replace(std::string part, std::string base_indent) {
        if (part.empty() || is_spaces(part)) {
            return "";
        }
        for (const auto &macro: macros) {
            std::smatch results;
            if (std::regex_search(part, results, std::regex(macro.name + R"(\<)"))) {
                size_t index = results.length();
                size_t depth = 1;
                while (depth) {
                    if (index == part.size()) {
                        throw std::runtime_error("No matching '>' found in line: " + part);
                    }
                    switch (part[index]) {
                        case '<':
                            depth++;
                            break;
                        case '>':
                            depth--;
                            break;
                        default:
                            break;
                    }
                    index++;
                }
                auto args = replace(part.substr(results.length(), index - results.length() - 1), "");
                try {
                    part = results.prefix().str() +
                           macro.try_replace_with(args, base_indent) +
                           part.substr(index);
                    changed = true;
                } catch (std::runtime_error &e) {}
            }
        }
        return part;
    }

    void scan_macro_defs() {
        string_list result;
        std::string name, macro_outer, form_args, form_outer, form_body;
        std::string cur_indent, line;
        std::vector<Form> forms;
        bool macro_started = false, form_started = false;

        const auto test_next_macro = [&result, &name, &macro_outer, &cur_indent, &line, &macro_started]() {
            std::smatch results;
            if (std::regex_search(line, results, RE_MACRO_DEF)) {
                name = results[1].str();
                macro_started = true;
                if (name.back() != '!') {
                    std::cerr << "Macro names should end with '!'. "
                              << "Consider changing \"" << name << "\" to \""
                              << name << "\"!" << std::endl;
                }
                macro_outer = cur_indent;
            } else {
                result.push_back(line);
                macro_started = false;
            }
        };

        const auto add_form = [&form_args, &form_body, &forms, &form_started]() {
            forms.emplace_back(form_args, form_body);
            form_started = false;
        };

        const auto test_next_form = [&form_outer, &form_args, &form_body, &form_started, &cur_indent, &line]() {
            std::smatch results;
            if (std::regex_search(line, results, RE_FORM_DEF)) {
                form_outer = cur_indent;
                if (results[1].str().empty()) {
                    form_args = "";
                } else {
                    form_args = results[2].str();
                }
                form_body.clear();
                form_started = true;
            } else {
                if (!is_debug_info(line)) {
                    std::cerr << "Stray line in macro definition: \""
                              << line << "\"" << std::endl;
                }
            }
        };

        auto lines = split(content, '\n');

        for (const auto &line_: lines) {
            line = line_;
            cur_indent = indent_of(line);
            if (!macro_started) {
                test_next_macro();
            } else {
                if (form_started) {
                    if (cur_indent.size() == form_outer.size()) {
                        add_form();
                        test_next_form();
                    } else if (cur_indent.size() <= form_outer.size()) {
                        add_form();
                        macros.emplace_back(name, forms);
                        macro_started = form_started = false;
                        forms.clear();
                        test_next_macro();
                    } else if (!is_debug_info(line)) {
                        form_body.append(line).push_back('\n');
                    }
                } else {
                    test_next_form();
                }
            }
        }
        content = join(result, '\n');
    }

    void replace_all() {
        changed = true;
        while (changed) {
            string_list result;
            changed = false;
            auto lines = split(content, '\n');
            for (auto line: lines) {
                line = replace(line, indent_of(line));
                result.push_back(line);
            }
            content = join(result, '\n');
        }
    }

public:
    MacroRules(const path &file) {
        content = read(file);
    }

    path work(const std::string &output_name) {
        scan_macro_defs();
        replace_all();

        path output = output_name + ".macro_rules.puppy";
        write(output, content);
        return output;
    }
};

PROCESSOR(macro_rules) {
    MacroRules macro_rules(file);
    return macro_rules.work(output_name);
}