//
// Created by impodog on 7/30/2023.
//
#include "processor.h"

template<typename type>
using string_map = std::unordered_map<std::string, type>;

bool requires_backslash(char c) {
    return isspace(c) || c == ':' || c == '\\';
}

std::regex RE_INT(R"(([^\w\d])(\d+)\b)"),
        RE_FLOAT(R"(([^\w\d])(\d+\.\d+)\b)"),
        RE_STRING(R"(([^\\])\"(.*?[^\\])\")"),
        RE_EMPTY_STRING(R"(([^\\])\"\")");

class Extract {
    std::string content;
    string_map<INT> ints;
    string_map<FLOAT> floats;
    string_map<std::string> strings;

    void extract_int() {
        string_list result;
        auto lines = split(content, '\n');
        for (auto line: lines) {
            std::smatch results;
            if (std::regex_search(line, results, RE_INT)) {
                auto name = globals->next_name("INT");
                bool negative = results[1].str() == "-";
                INT value = std::stoi(results[2].str());
                if (negative) {
                    ints.insert({name, -value});
                    line = results.prefix().str() + name + results.suffix().str();
                } else {
                    ints.insert({name, value});
                    line = results.prefix().str() + results[1].str()
                           + name + results.suffix().str();
                }
            }
            result.push_back(line);
        }
        content = join(result, '\n');
    }

    void extract_float() {
        string_list result;
        auto lines = split(content, '\n');
        for (auto line: lines) {
            std::smatch results;
            if (std::regex_search(line, results, RE_FLOAT)) {
                auto name = globals->next_name("FLOAT");
                bool negative = results[1].str() == "-";
                FLOAT value = std::stod(results[2].str());
                if (negative) {
                    floats.insert({name, -value});
                    line = results.prefix().str() + name + results.suffix().str();
                } else {
                    floats.insert({name, value});
                    line = results.prefix().str() + results[1].str()
                           + name + results.suffix().str();
                }
            }
            result.push_back(line);
        }
        content = join(result, '\n');
    }

    void extract_string() {
        string_list result;
        auto lines = split(content, '\n');
        for (auto line: lines) {
            std::smatch results;
            if (std::regex_search(line, results, RE_STRING)) {
                auto name = globals->next_name("STR");
                auto value = results[2].str();
                if (requires_backslash(value.back())) {
                    value.push_back('\\');
                }
                if (requires_backslash(value.front())) {
                    value = "\\" + value;
                }
                strings.insert({name, value});
                line = results.prefix().str() + results[1].str()
                       + name + results.suffix().str();
            } else if (std::regex_search(line, results, RE_EMPTY_STRING)) {
                auto name = globals->next_name("STR");
                strings.insert({name, ""});
                line = results.prefix().str() + results[1].str()
                       + name + results.suffix().str();
            }
            result.push_back(line);
        }
        content = join(result, '\n');
    }

    void output_extraction(const path &output) {
        std::ofstream out(output);
        out << "INT:\n";
        for (auto &item: ints) {
            out << item.first << " = " << item.second << "\n";
        }
        out << "FLOAT:\n";
        for (auto &item: floats) {
            out << item.first << " = " << item.second << "\n";
        }
        out << "STR:\n";
        for (auto &item: strings) {
            out << item.first << " = " << item.second << "\n";
        }
        out.close();
    }

public:
    explicit Extract(const path &file) {
        content = read(file);
    }

    path work(const std::string &output_name) {
        extract_int();
        extract_float();
        extract_string();
        output_extraction(output_name + CONST_SUFFIX);

        path output = output_name + ".extract.puppy";
        write(output, content);
        return output;
    }
};

PROCESSOR(extract) {
    Extract extract(file);
    return extract.work(output_name);
}