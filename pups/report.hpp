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
            Report_UnConstInit = "UnConstInit",
            Report_LocalityErr = "LocalityErr",
            Report_FileNotFound = "FileNotFound",
            Report_SuspiciousLine = "SuspiciousLine",
            Report_AgainWarning = "AgainWarning",
            Report_NewWarning = "NewWarning",
            Report_Redundant = "Redundant";

    class ObjectBase;

    class Report {
        const size_t &line;
        fpath _file;
        const std::string &cur_line;
        struct ReportMsg {
            size_t line;
            std::string head, body;
            const fpath &file;
            const std::string &cur_line;
        };
        std::queue<ReportMsg> reports;

        static void show_msg(const ReportMsg &msg) {
            *output << "[" << msg.head << " in " << msg.file << " line " << msg.line
                    << "]\nEXECUTING:\n" << msg.cur_line << "\nREPORT:\n" << msg.body << "\n";
        }

    public:
        std::list<fpath> paths;
        static std::ostream *output;

        Report(const size_t &line, fpath file, const std::string &cur_line) : line(line),
                                                                              _file(std::move(file)),
                                                                              cur_line(cur_line) {
            paths.push_back(_file.parent_path());
            paths.emplace_back(std_path);
        }

        explicit Report(const TokenInput &input) : Report{input.line_num(), input.file(), input.cur_line()} {}

        Report(const Report &report) : Report{report.line, report._file, report.cur_line} {}

        [[nodiscard]] std::string where() const {
            return "in " + _file.string() + " line " + std::to_string(line) + "\nEXECUTING:\n" + cur_line;
        }

        [[nodiscard]] std::string file() const noexcept {
            return _file.string();
        }

        void report(std::string head, std::string body) {
            reports.push({line, std::move(head), std::move(body), _file, cur_line});
        }

        [[nodiscard]] bool no_report() const noexcept {
            return reports.empty();
        }

        void release() {
            show_msg(reports.front());
            reports.pop();
        }

        void release_all() {
            while (!no_report())
                release();
        }

        void append(Report &report) {
            while (!report.reports.empty()) {
                reports.push(report.reports.front());
                report.reports.pop();
            }
        }

        fpath find_file(const std::string &file) {
            for (auto path: paths) {
                path.append(file);
                if (std::filesystem::exists(path)) return absolute(path);
                path += ".pups";
                if (std::filesystem::exists(path)) return absolute(path);
            }
            return {};
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
