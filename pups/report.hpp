//
// Created by Dogs-Cute on 4/22/2023.
//

#ifndef POURUPSCRIPT_REPORT_HPP
#define POURUPSCRIPT_REPORT_HPP

#include <utility>

#include "token.hpp"


namespace PUPS {
    constexpr const cstr
            Report_NullErr = "NullErr",
            Report_UnknownName = "UnknownName",
            Report_EmptyCompound = "EmptyCompound",
            Report_AbnormalStack = "AbnormalStack",
            Report_WrongToken = "WrongToken",
            Report_ReDeclaring = "Re-declaring",
            Report_Uninitialized = "Uninitialized",
            Report_IncorrectArguments = "IncorrectArguments",
            Report_OperatorErr = "OperatorErr",
            Report_Undeclared = "Undeclared",
            Report_TypeErr = "TypeErr",
            Report_SuggestMakeInit = "SuggestMakeInit",
            Report_LocalityErr = "LocalityErr",
            Report_FileNotFound = "FileNotFound",
            Report_SuspiciousLine = "SuspiciousLine",
            Report_AgainWarning = "AgainWarning",
            Report_NewWarning = "NewWarning",
            Report_Redundant = "Redundant",
            Report_AnyTErr = "AnyTErr";

    class ObjectBase;

    class Report {
        TokenInput input;
        std::string name;
        struct ReportMsg {
            size_t line;
            std::string head, body;
            fpath file;
            std::string cur_line;
        };
        std::queue<ReportMsg> *reports;
        Report *parent = nullptr;

        static void show_msg(const ReportMsg &msg) {
            *output << "[" << msg.head << " in " << msg.file << " line " << msg.line
                    << "]\nEXECUTING:\n" << msg.cur_line << "\nREPORT:\n" << msg.body << "\n";
        }

    public:
        std::list<fpath> *paths;
        static std::ostream *output;

        explicit Report(const fpath &path) : input(path), name(path.string()), reports(new std::queue<ReportMsg>),
                                             paths(new std::list<fpath>) {
            paths->push_back(std::filesystem::absolute(path.parent_path()));
            paths->push_back("./std");
            paths->push_back(".");
        }

        Report(Report &report, const fpath &p) : input(p), name(report.name), reports(report.reports),
                                                 paths(report.paths),
                                                 parent(&report) {}

        Report(Report &report, const Token &t) : input(t.str_dependent()), name(report.name), reports(report.reports),
                                                 paths(report.paths),
                                                 parent(&report) {}

        Report(Report &report, const std::string &s) : input(s), name(report.name), reports(report.reports),
                                                       paths(report.paths),
                                                       parent(&report) {}

        ~Report() {
            if (!parent) {
                release_all();
                delete reports;
                delete paths;
            }
        }

        [[nodiscard]] std::string where() const {
            return "in " + name + " line " + std::to_string(input.line_num()) + "\nEXECUTING:\n" + input.cur_line();
        }

        [[nodiscard]] std::string file() const noexcept {
            return name;
        }

        void report(std::string head, std::string body) {
            reports->push({input.line_num(), std::move(head), std::move(body), name, input.cur_line()});
        }

        [[nodiscard]] bool no_report() const noexcept {
            return reports->empty();
        }

        void release() {
            show_msg(reports->front());
            reports->pop();
        }

        void release_all() {
            while (!no_report())
                release();
        }

        [[nodiscard]] fpath find_file(const std::string &file) const {
            for (auto path: *paths) {
                path.append(file);
                if (std::filesystem::exists(path)) return absolute(path);
                path += ".pups";
                if (std::filesystem::exists(path)) return absolute(path);
            }
            return {};
        }

        Token next() {
            return input.next();
        }
    };

    std::ostream *Report::output;

    class PUPS_FatalError final : public std::runtime_error {
        Report &report;
    public:
        explicit PUPS_FatalError(const char *msg, Report &report) : std::runtime_error(report.where() + ", " + msg),
                                                                    report(report) {}

        explicit PUPS_FatalError(const std::string &msg, Report &report) : std::runtime_error(
                report.where() + ", " + msg), report(report) {}

        [[nodiscard]] const char *what() const noexcept override {
            auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            *Report::output
                    << std::put_time(std::localtime(&time), "[%m/%d/%Y %H:%M:%S] When PUPS_FatalError occurred:")
                    << "\n";
            report.release_all();
            return std::runtime_error::what();
        }
    };
}
#undef PUPS_FATAL

#endif //POURUPSCRIPT_REPORT_HPP
