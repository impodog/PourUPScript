//
// Created by impodog on 6/3/2023.
//

#include "constants.h"
#include "builtins/numbers.h"
#include "builtins/strings.h"

namespace pups::library {
    ObjectPtr sym_assign = std::make_shared<Symbol>();

    void to_lower(std::string &s) {
        for (auto &c: s)
            c = static_cast<char>(tolower(c));
    }

    void split(const std::string &source, std::string &front, std::string &back) {
        auto index = source.find('=');
        front = source.substr(0, index);
        back = source.substr(index + 1);
    }

    std::string ltrim(const std::string &s) {
        size_t start = s.find_first_not_of(' ');
        return (start == std::string::npos) ? "" : s.substr(start);
    }

    std::string rtrim(const std::string &s) {
        size_t end = s.find_last_not_of(' ');
        return (end == std::string::npos) ? "" : s.substr(0, end + 1);
    }

    std::string trim(const std::string &s) {
        return rtrim(ltrim(s));
    }

    void split_trim(const std::string &source, std::string &front, std::string &back) {
        split(source, front, back);
        front = trim(front);
        back = trim(back);
    }

    void Constants::load_from(const path &path) {
        std::ifstream ifs(path);
        std::string line;
        while (!ifs.eof()) {
            std::getline(ifs, line);
            run_line(line);
        }
    }

    void Constants::run_line(const std::string &line) {
        if (line.back() == ':')
            switch_status(line.substr(0, line.size() - 1));
        else {
            std::string front, back;
            split_trim(line, front, back);
            if (!front.empty() && !back.empty())
                try {
                    switch (stat) {
                        case stat_none:
                            std::cout << "Line \"" << line << "\" is skipped because no status is set." << std::endl;
                            break;
                        case stat_int:
                            add(Id{"", front}, std::make_shared<builtins::numbers::NumType<int>>(std::stoi(back)));
                            break;
                        case stat_float:
                            add(Id{"", front}, std::make_shared<builtins::numbers::NumType<float>>(std::stof(back)));
                            break;
                        case stat_str:
                            add(Id{"", front}, std::make_shared<builtins::strings::String>(back));
                            break;
                    }
                } catch (const std::invalid_argument &exception) {
                    throw std::runtime_error(
                            "Cannot analyze constant file line: \"" + line + "\" as follows: " + exception.what());
                }
        }
    }

    void Constants::switch_status(std::string status) {
        to_lower(status);
        try {
            stat = status_table.at(status);
        } catch (const std::out_of_range &) {
            throw std::runtime_error("Unknown status mark : \"" + status + "\".");
        }
    }

    Constants::Constants(const path &path) :
            constants({{Id{"", "="}, sym_assign}}) {
        load_from(path);
    }

    void Constants::export_to(Map *map) {
        for (auto &constant: constants) {
            map->add_object(constant.first, constant.second);
        }
    }

    void Constants::add(const Id &id, const ObjectPtr &object) {
        constants.insert({id, object});
    }
}
