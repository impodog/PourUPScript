//
// Created by Dogs-Cute on 4/22/2023.
//

#ifndef POURUPSCRIPT_MANAGE_HPP
#define POURUPSCRIPT_MANAGE_HPP

#include "keywords.hpp"

namespace PUPS {
    class Scripter {
    protected:
        TokenInput tokenInput;
        Report _report;
        Scope script_scope;
    public:
        Scripter(const std::string &file, Keywords &keywords) :
                tokenInput(file), _report(tokenInput), script_scope(tokenInput, nullptr) {
            keywords.merge_to(script_scope);
        }

        ~Scripter() {
            script_scope.exit(_report);
            _report.append(script_scope.reports());
            if (!_report.no_report()) {
                *Report::output << "Scripter hit the end with reports:\n";
                report_all();
            }
        }

        /*Go forward until next end of statement*/
        bool forward() {
            bool end = false;
            while (!end) {
                auto token = tokenInput.next();
                if (token.eof()) return false;
                script_scope.put(token, _report);
                end = token.is_symbol() && token.semicolon();
            }
            script_scope.ends(&script_scope, _report);
            return true;
        }

        /*Output report for certain times. Safe to call with any number because it quits when no report is present*/
        void report(size_t times = 1) noexcept {
            for (size_t i = 0; i < times; i++) {
                if (_report.no_report()) return;
                _report.release();
            }
        }

        void report_all() noexcept {
            _report.release_all();
        }

        Report &get_report() noexcept {
            return _report;
        }
    };
}

#endif //POURUPSCRIPT_MANAGE_HPP
