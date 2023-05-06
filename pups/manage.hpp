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

    ObjectPtr make_scope(const Token &name, const fpath &path, Scope *parent, Report &report,
                         const std::unordered_map<Token, Token> &args) {
        std::shared_ptr<Scope> scope;
        {
            auto result = parent->find<true>(name);
            if (is_non(result)) {
                scope = std::make_shared<Scope>(parent, report);
            } else if (result->is_scope()) {
                scope = std::static_pointer_cast<Scope>(result);
            } else {
                report.report(Report_TypeErr, "Scope re-entering must point to a previous scope. Action skipped.");
                return null_obj;
            }
        }
        // Add arguments
        for (const auto &arg: args)
            scope->set_object<true>(arg.first, scope->find(arg.second));

        Report new_report(path);
        while (forward(scope.get(), new_report)) {
            new_report.release_all();
        }
        add_scope(name, scope, parent);

        // Destroy arguments
        for (const auto &arg: args)
            scope->erase_object(arg.first);
        scope->temporary_exit();
        return scope;
    }

    inline ObjectPtr make_scope(const Token &name, const Token &block, Scope *parent, Report &report,
                                const std::unordered_map<Token, Token> &args) {
        if (!block.is_long()) {
            report.report(Report_WrongToken, "Cannot make Scope using non-long strings.");
            return null_obj;
        }
        std::string file = next_file_s(report.file());
        {
            std::ofstream out(file, std::ios::out);
            out << block.str_dependent() << std::endl;
        }
        auto scope = make_scope(name, file, parent, report, args);
        if (!keepTemporary)
            std::filesystem::remove(file);
        return scope;
    }
}

#endif //POURUPSCRIPT_MANAGE_HPP
