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
#define Report_UnConstInit "UnConstInit"

namespace PUPS {
    class ObjectBase;

    class Report {
        const size_t &line;
        const std::string &file, &cur_line;
        struct ReportMsg {
            size_t line;
            std::string head, body;
            const std::string &file, &cur_line;
        };
        std::queue<ReportMsg> reports;

        static void show_msg(const ReportMsg &msg) {
            *output << "[" << msg.head << " in " << msg.file << " line " << msg.line
                    << "]\n\tat \"" << msg.cur_line << "\"\n\t" << msg.body << "\n";
        }

    public:
        static std::ostream *output;

        Report(const size_t &line, const std::string &file, const std::string &cur_line) : line(line),
                                                                                           file(file),
                                                                                           cur_line(cur_line) {}

        explicit Report(const TokenInput &input) : Report{input.line_num(), input.file(), input.cur_line()} {}

        [[nodiscard]] std::string where() const {
            return "in " + file + " line " + std::to_string(line) + "\n\tat \"" + cur_line + "\"";
        }

        void report(std::string head, std::string body) {
            reports.push({line, std::move(head), std::move(body), file, cur_line});
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
