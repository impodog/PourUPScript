//
// Created by Dogs-Cute on 4/22/2023.
//

#ifndef POURUPSCRIPT_MANAGE_HPP
#define POURUPSCRIPT_MANAGE_HPP

#include <memory>

#include "keywords.hpp"

namespace PUPS {
    class Scripter {
        bool ind_open;
    protected:
        Report _report;
        std::shared_ptr<Scope> script_scope;
    public:

        Scripter(const fpath &file, Keywords &keywords,
                 const std::shared_ptr<Scope> &scope = std::shared_ptr<Scope>()) :
                _report(file), script_scope(scope), ind_open(true) {
            if (!script_scope)
                script_scope = std::make_shared<Scope>(nullptr, _report);
            keywords.merge_to(*script_scope);
        }

        ~Scripter() {
            if (ind_open)
                script_scope->exit(_report);
            if (!_report.no_report()) {
                *Report::output << "Scripter hit the end with get_report:\n";
                report_all();
            }
        }

        /*Go forward until next end of statement*/
        bool forward() {
            return ::PUPS::forward(script_scope.get(), _report);
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

        [[nodiscard]] std::shared_ptr<Scope> get_scope() const noexcept {
            return script_scope;
        }

        void add_prior_path(const fpath &path) const {
            _report.paths->push_front(path);
        }

        void add_least_path(const fpath &path) const {
            _report.paths->push_back(path);
        }
    };
}

#endif //POURUPSCRIPT_MANAGE_HPP
