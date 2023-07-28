//
// Created by impodog on 6/18/2023.
//

#include "sys.h"

namespace pups::modules::sys {
    struct Version {
        int major, minor, bugfix;

        explicit Version(const std::string &ver) {
            int status = 0;
            std::string tmp;
            for (auto c: ver) {
                if (c == '.') {
                    switch (status) {
                        case 0:
                            major = std::stoi(tmp);
                            break;
                        case 1:
                            minor = std::stoi(tmp);
                            break;
                        default:
                            throw std::invalid_argument("Too many dots in a version.");
                    }
                    status++;
                    tmp.clear();
                } else
                    tmp.push_back(c);
            }
            if (status == 2)
                bugfix = std::stoi(tmp);
            else
                throw std::invalid_argument("Too few dots in a version.");
        }

        bool operator>=(const Version &ver) const noexcept {
            if (major < ver.major) return false;
            if (minor < ver.minor) return false;
            if (bugfix < ver.bugfix) return false;
            return true;
        }
    };

    ObjectPtr is_toplevel(FunctionArgs &args, Map *map) {
        return (map->get_parent() && map->get_parent()->get_parent() == nullptr)
               ? numbers::True : numbers::False;
    }

    ObjectPtr version_compare(FunctionArgs &args, Map *map) {
        if (args.size() != 2)
            map->throw_error(std::make_shared<library::ArgumentError>("sys.ver_cmp requires two only arguments."));
        else {
            auto lhs = cast<strings::String>(*args.front());
            args.pop_front();
            auto rhs = cast<strings::String>(*args.front());
            args.pop_front();
            if (lhs && rhs) {
                try {
                    Version left{lhs->data()}, right{rhs->data()};
                    return left >= right ? numbers::True : numbers::False;
                } catch (const std::invalid_argument &) {
                    map->throw_error(
                            std::make_shared<library::ValueError>("sys.ver_cmp got incorrect formats of version, "
                                                                  "which are " + lhs->repr() + " and " + rhs->repr() +
                                                                  "."));
                }
            } else
                map->throw_error(std::make_shared<library::TypeError>("sys.ver_cmp requires two string arguments."));
        }
        return pending;
    }

    Id id_sys = pups_std::get_std_lib_name("sys");
    Id id_is_toplevel{"", "is_toplevel"}, id_version{"", "version"}, id_version_compare{"", "ver_cmp"};

    void init(pups::Constants &constants) {
        auto &sys = constants.new_sub_const(id_sys);
        sys.add(id_is_toplevel, std::make_shared<Function>(is_toplevel));
        sys.add(id_version, std::make_shared<String>(PUPS_LIB_VERSION));
        sys.add(id_version_compare, std::make_shared<Function>(version_compare));
    }
}