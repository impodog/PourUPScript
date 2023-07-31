//
// Created by impodog on 7/30/2023.
//

#include "processor.h"

class Command {
    std::regex src;
    std::string target;
    bool word;

public:
    Command(std::string src, std::string target) :
            target(std::move(target)) {
        word = is_word(src);
        if (word) {
            this->src = std::regex(R"(#*(\b)()" + src + R"()(\b)#*)");
        } else {
            this->src = std::regex(src);
        }
    }

    std::string sub(std::string s) {
        if (word) {
            while (true) {
                std::smatch results;
                if (!std::regex_search(s, results, src)) {
                    return s;
                }
                std::string tmp;
                if (results[1].matched) {
                    tmp = results.prefix().str() + target;
                } else {
                    tmp = results.prefix().str() + results[2].str() + target;
                }
                if (results[3].matched) {
                    tmp += results.suffix().str();
                } else {
                    tmp += results[3].str() + results.suffix().str();
                }
                s = std::move(tmp);
            }
        } else {
            return std::regex_replace(s, src, target);
        }
    }

    void sub(Command &cmd) {
        cmd.target = sub(cmd.target);
    }
};

std::string add_debug_info(const std::string &content, const path &file) {
    std::string file_name = file.string();
    std::string prev_line;

    string_list result;
    size_t line_num = 0;

    auto lines = split(content, '\n');
    for (const auto &line: lines) {
        line_num++;
        if (line.empty() || is_spaces(line)
            || starts_with(left_strip(line), "#")
            || ends_with(right_strip(prev_line), "\\")) {
            prev_line = line;
            continue;
        }
        result.push_back(
                indent_of(line) + DEBUG_INFO_STMT " \"FILE \\\"" + file_name + "\\\", LINE " +
                std::to_string(line_num) + "\"");
        result.push_back(line);
        prev_line = line;

    }

    return join(result, '\n');
}

const std::regex RE_INCLUDE(R"(\s*#include\s+(.+?)\s*)"),
        RE_BLOCK_COMMENT(R"(/\*.*?\*/)"),
        RE_LINE_COMMENT(R"(//.*)"),
        RE_DEFINE(R"(\s*#define\s+(.+?)\s*=\s*(.+?)\s*)"),
        RE_UNDEF(R"(\s*#undef\s+(.+?)\s*)"),
        RE_RULE(R"(\s*#rule\s+(.+?)\s*=\s*(.+?)\s*)");

class Preprocessor {
    std::string content;
    path file_dir;
    std::unordered_map<std::string, Command> commands;
    bool debug_mode;

    static path get_include_path(const path &dir, const std::string &file, std::string &file_content) {
        try {
            path result = dir / file;
            file_content = read(result);
            return result;
        } catch (const std::runtime_error &) {}
        try {
            path result = dir / "include" / file;
            file_content = read(result);
            return result;
        } catch (const std::runtime_error &) {}
        try {
            path result = dir / "include" / (file + ".puppy");
            file_content = read(result);
            return result;
        } catch (const std::runtime_error &) {}
        throw std::runtime_error("Cannot include file : " + file);
    }

    void scan_includes() {
        bool changed = true;
        while (changed) {
            changed = false;
            string_list result;
            auto lines = split(content, '\n');
            for (const auto &line: lines) {
                std::smatch results;
                if (std::regex_match(line, results, RE_INCLUDE)) {
                    changed = true;
                    auto file = results[1].str();
                    std::string file_content;
                    auto include_path = get_include_path(file_dir, file, file_content);
                    if (debug_mode) {
                        file_content = add_debug_info(file_content, include_path);
                    }
                    result.push_back(file_content);
                } else {
                    result.push_back(line);
                }
            }
            content = join(result, '\n');
        }
    }

    void remove_comments() {
        content = std::regex_replace(content, RE_BLOCK_COMMENT, "");
        content = std::regex_replace(content, RE_LINE_COMMENT, "");
    }

    void format_blanks() {
        string_list result;
        auto lines = split(content, '\n');
        for (auto line: lines) {
            line = right_strip(line);
            if (!line.empty()) {
                result.push_back(line);
            }
        }
        content = join(result, '\n');
    }

    void scan_line_continuation() {
        string_list result;
        auto lines = split(content, '\n');
        for (const auto &line: lines) {
            if (ends_with(line, "\\")) {
                result.push_back(line.substr(0, line.size() - 1));
            } else {
                result.push_back(line);
            }
        }
        content = join(result, '\n');
    }

    /* This function handles the following macros:
     * #set <NAME>... => Appends the names into defined set
     * #unset <NAME>... => Remove the names from defined set, if present
     * #ifset <NAME>... => Matches the corresponding #end, will only add code when all the names are defined
     * #ifnset <NAME>... => Matches the corresponding #end, will only add code when all the names are not defined
     * #end => Marks an end to #ifdef and #ifndef blocks. The blocks can contain other blocks*/
    void scan_set_macros() {
        // This function is deprecated.
        // Set macros aren't very useful, and can be replaced
    }

    void scan_define_macros() {
        string_list result;
        auto lines = split(content, '\n');
        for (auto line: lines) {
            std::smatch results;
            if (std::regex_match(line, results, RE_DEFINE)) {
                std::string name = results[1].str();
                std::string value = results[2].str();
                Command cmd{name, value};
                for (auto &cmd: commands) {
                    cmd.second.sub(value);
                }
                commands.insert({name, std::move(cmd)});
            } else if (std::regex_match(line, results, RE_UNDEF)) {
                std::string name = results[1].str();
                try {
                    commands.erase(name);
                } catch (const std::out_of_range &) {}
            } else {
                for (auto &cmd: commands) {
                    line = cmd.second.sub(line);
                }
                result.push_back(line);
            }
        }
        content = join(result, '\n');
    }

    void scan_rule_macros() {
        string_list result;
        auto lines = split(content, '\n');
        for (const auto &line: lines) {
            std::smatch results;
            if (std::regex_match(line, results, RE_RULE)) {
                auto name = results[1].str();
                auto value = results[2].str();
                Rule rule = rule_names.at(name);
                int val;
                if (value == "true") {
                    val = 1;
                } else if (value == "false") {
                    val = 0;
                } else if (value == "show") {
                    std::cout << "Rule " << name << ", value is "
                              << globals->rules.at(rule)
                              << std::endl;
                    continue;
                } else if (value == "default") {
                    val = default_rules.at(rule);
                } else {
                    val = std::stoi(value);
                }
                globals->rules.at(rule) = val;
            } else {
                result.push_back(line);
            }
        }
        content = join(result, '\n');
    }

public:
    Preprocessor(const path &file, bool debug_mode) :
            debug_mode(debug_mode) {
        content = read(file);
        file_dir = file.parent_path();
        if (debug_mode) {
            content = add_debug_info(content, file);
        }
    }

    path work(const std::string &output_name) {
        scan_includes();
        remove_comments();
        format_blanks();
        scan_line_continuation();
        scan_define_macros();
        scan_rule_macros();

        path output = output_name + ".preprocess.puppy";
        write(output, content);
        return output;
    }

};

PROCESSOR(preprocess) {
    Preprocessor preprocessor(file, debug_mode);
    return preprocessor.work(output_name);
}
