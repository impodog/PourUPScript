//
// Created by Dogs-Cute on 4/22/2023.
//

#ifndef POURUPSCRIPT_REPORT_HPP
#define POURUPSCRIPT_REPORT_HPP

#include <utility>

#include "token.hpp"

#define Report_NullErr "NullErr"
#define Report_UnknownName "UnknownName"
#define Report_EmptyCompound "EmptyCompound"
#define Report_AbnormalStack "AbnormalStack"
#define Report_WrongToken "WrongToken"
#define Report_ReDeclaring "Re-declaring"
#define Report_Uninitialized "Uninitialized"
#define Report_IncorrectArguments "IncorrectArguments"
#define Report_OperatorErr "OperatorErr"
#define Report_Undeclared "Undeclared"
#define Report_TypeErr "TypeErr"

namespace PUPS {
    class ObjectBase;

    class Report {
        const size_t &line, &column;
        const std::string &file;
        struct ReportMsg {
            size_t line, column;
            std::string head, body;
            const std::string &file;
        };
        std::queue<ReportMsg> reports;

        static void show_msg(const ReportMsg &msg) {
            *output << "[" << msg.head << " in " << msg.file << " " << msg.line << ":" << msg.column
                    << "] " << msg.body << "\n";
        }

    public:
        static std::ostream *output;

        Report(const size_t &line, const size_t &column, const std::string &file) : line(line), column(column),
                                                                                    file(file) {}

        explicit Report(const TokenInput &input) : Report{input.line_num(), input.column_num(), input.file()} {}

        [[nodiscard]] std::string where() const {
            return "in " + file + " " + std::to_string(line) + ":" + std::to_string(column);
        }

        void report(std::string head, std::string body) {
            reports.push({line, column, std::move(head), std::move(body), file});
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
