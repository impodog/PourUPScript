//
// Created by impodog on 6/3/2023.
//

#include "constants.h"
#include "builtins/types/numbers.h"
#include "builtins/types/strings.h"

namespace pups::library {
    constexpr cstr sym_name_assign = "assign",
            sym_name_dynamic = "dynamic";
    ObjectPtr sym_assign = std::make_shared<Symbol>(sym_name_assign),
            sym_dynamic = std::make_shared<Symbol>(sym_name_dynamic);

    path get_constants_path(const path &path) {
        return path.parent_path().string() + "/" + path.stem().string() + ".con";
    }

    void to_lower(std::string &s) {
        for (auto &c: s)
            c = static_cast<char>(tolower(c));
    }

    void split(const std::string &source, std::string &front, std::string &back) {
        auto index = source.find('=');
        front = source.substr(0, index);
        back = source.substr(index + 1);
    }

    void replace_all(std::string &source, const char *from, const char *to) {
        size_t ret_pos;
        do {
            ret_pos = source.find(from);
            if (ret_pos == std::string::npos)
                break;
            source.replace(ret_pos, 2, to);
        } while (true);
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

    void Constants::run_line(const std::string &line) {
        if (line.back() == ':')
            switch_status(line.substr(0, line.size() - 1));
        else
            analyze_line(line);
    }

    void Constants::switch_status(std::string status) {
        to_lower(status);
        try {
            stat = status_table.at(status);
        } catch (const std::out_of_range &) {
            throw std::runtime_error("Unknown status mark : \"" + status + "\".");
        }
    }

    void Constants::analyze_line(const std::string &line) {
        std::string front, back;
        split_trim(line, front, back);
        if (front.empty()) return;
        try {
            switch (stat) {
                case stat_none:
                    std::cout << "Line \"" << line << "\" is skipped because no status is set." << std::endl;
                    break;
                case stat_int:
                    add(Id{"", front}, std::make_shared<builtins::numbers::IntType>(std::stoi(back)));
                    break;
                case stat_float:
                    add(Id{"", front}, std::make_shared<builtins::numbers::FloatType>(std::stof(back)));
                    break;
                case stat_str:
                    if (back.front() == '\\')
                        back = back.substr(1);
                    if (back.back() == '\\')
                        back = back.substr(0, back.size() - 1);
                    replace_all(back, "\\n", "\n");
                    add(Id{"", front}, std::make_shared<builtins::strings::String>(back));
                    break;
            }
        } catch (const std::invalid_argument &exception) {
            throw std::runtime_error(
                    "Cannot analyze constant file line: \"" + line + "\" as follows: " + exception.what());
        }
    }

    MapPtr Constants::make_sub_map(Map *map) {
        return std::make_shared<Map>(map, false);
    }

    Constants::Constants(const path &path) {
        load_from(path);
    }

    Constants::Constants(std::initializer_list<path> path) {
        for (const auto &p: path)
            load_from(p);
    }

    void Constants::load_from(const path &path) {
        std::ifstream ifs(path);
        std::string line;
        if (!ifs.is_open())
            throw std::invalid_argument("File \"" + path.string() + "\" is not found.");
        while (!ifs.eof()) {
            std::getline(ifs, line);
            run_line(line);
        }
    }

    void Constants::export_to(Map *map) {
        for (auto &constant: constants) {
            map->add_object(constant.first, constant.second);
        }
        for (auto &sub_const: sub_consts) {
            MapPtr sub_map = make_sub_map(map);
            map->set_child(nullptr);
            sub_const.second->export_to(sub_map.get());
            map->add_object(sub_const.first, sub_map);
        }
    }

    void Constants::add(const Id &id, const ObjectPtr &object) {
        constants.insert({id, object});
    }

    Constants &Constants::new_sub_const(const Id &id) {
        auto result = std::make_shared<Constants>(std::initializer_list<path>{});
        sub_consts.insert({id, result});
        return *result;
    }
}
