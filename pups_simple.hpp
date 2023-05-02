//
// Created by Dogs-Cute on 5/2/2023.
//
#ifndef POURUPSCRIPTINTERP_PUPS_SIMPLE_HPP
#define POURUPSCRIPTINTERP_PUPS_SIMPLE_HPP

#include <memory>

#include "cpp_lib/cpp_lib.h"
#include "pups/manage.hpp"
#include "std_lib/include.hpp"

namespace PUPS::Simple {
    namespace Globals {
        constexpr ObjectPtr &Null = null_obj;
        constexpr ObjectPtr &False = false_obj;
        constexpr ObjectPtr &True = true_obj;
    }
    std::unique_ptr<Keywords> keywords;
    static bool inited = false;

    /*Initialize the interpreter using a simple function. Can be called many times.*/
    void init() {
        if (!inited) {
            Report::output = &std::cout;
            keywords = std::make_unique<Keywords>();
            INCLUDE_ALL();
            inited = true;
        }
    }

    struct InterpFile {
        std::unique_ptr<Scripter> scripter;

        explicit InterpFile(const fpath &path) : scripter(new Scripter(path, *keywords)) {}

        ~InterpFile() = default;

        void run() const {
            while (scripter->forward()) {
                scripter->report_all();
            }
        }

        void report() const {
            scripter->report_all();
        }

        [[nodiscard]] bool step() const {
            return scripter->forward();
        }

        [[nodiscard]] ObjectPtr &find(const std::string &name) const {
            return scripter->get_scope()->find(Token{name});
        }

        FN_ALIAS(at, find)

        [[nodiscard]] EvalResult find_v(const std::string &name) const {
            return find(name)->get_ev();
        }

        void set(const std::string &name, const ObjectPtr &obj) const {
            scripter->get_scope()->set_object(Token{name}, obj);
        }
    };

}

#endif //POURUPSCRIPTINTERP_PUPS_SIMPLE_HPP
