//
// Created by Dogs-Cute on 5/7/2023.
//

#ifndef POURUPSCRIPTINTERP_LIB_VERSION_INFO_HPP
#define POURUPSCRIPTINTERP_LIB_VERSION_INFO_HPP

#include "../pups.h"

namespace PUPS::Std_VersionInfo {
    MAKE_BUILTIN(VERSION_INFO_version) {
        if (!args.empty())
            report.report("VERSION_INFO_version", "No args should be received.");
        return std::make_shared<INST_Str>(TypeCodes::Str, version);
    }

    MAKE_BUILTIN(VERSION_INFO_major) {
        if (!args.empty())
            report.report("VERSION_INFO_major", "No args should be received.");
        return std::make_shared<INST_Str>(TypeCodes::Str, major_version);
    }

    MAKE_BUILTIN(VERSION_INFO_minor) {
        if (!args.empty())
            report.report("VERSION_INFO_minor", "No args should be received.");
        return std::make_shared<INST_Str>(TypeCodes::Str, minor_version);
    }

    MAKE_BUILTIN(VERSION_INFO_patch) {
        if (!args.empty())
            report.report("VERSION_INFO_patch", "No args should be received.");
        return std::make_shared<INST_Str>(TypeCodes::Str, patch_version);
    }

#define VERSION_INFO_FUNCTION(name, cond) {\
        if (args.size() != 1)\
            report.report(name, "Args should be 1.");\
        EvalResult result = (*args.front())->get_ev();\
        if (result.type != EvalResult::type_str) {\
            report.report(name, "Arg should be a string. False returned.");\
            return false_obj;\
        }\
        return cond(*result.result.r_str, report) ? true_obj : false_obj;\
    }
#define IS_SAME(str, report) version == str

    MAKE_BUILTIN(VERSION_INFO_is_same) VERSION_INFO_FUNCTION("VERSION_INFO_is_same", IS_SAME)

    bool is_gt(const std::string &ver, Report &report) {
        auto vers = split_by(ver, '.');
        if (vers.size() != 3) report.report("VERSION_INFO_is_gt", "Invalid version string.");
        while (!vers.empty()) {
            switch (vers.size()) {
                case 3:
                    if (major_version > vers.front()) return true;
                    break;
                case 2:
                    if (minor_version > vers.front()) return true;
                    break;
                case 1:
                    if (patch_version > vers.front()) return true;
                    break;
            }
            vers.pop();
        }
        return false;
    }

    MAKE_BUILTIN(VERSION_INFO_is_gt) VERSION_INFO_FUNCTION("VERSION_INFO_is_gt", is_gt)

    bool is_lt(const std::string &ver, Report &report) {
        auto vers = split_by(ver, '.');
        if (vers.size() != 3) report.report("VERSION_INFO_is_gt", "Invalid version string.");
        while (!vers.empty()) {
            switch (vers.size()) {
                case 3:
                    if (major_version < vers.front()) return true;
                    break;
                case 2:
                    if (minor_version < vers.front()) return true;
                    break;
                case 1:
                    if (patch_version < vers.front()) return true;
                    break;
            }
            vers.pop();
        }
        return false;
    }

    MAKE_BUILTIN(VERSION_INFO_is_lt) VERSION_INFO_FUNCTION("VERSION_INFO_is_lt", is_lt)

#define IS_GE(str, report) (is_gt(str, report) || IS_SAME(str, report))

    MAKE_BUILTIN(VERSION_INFO_is_ge) VERSION_INFO_FUNCTION("VERSION_INFO_is_ge", IS_GE)

#undef IS_GE

#define IS_LE(str, report) (is_lt(str, report) || IS_SAME(str, report))

    MAKE_BUILTIN(VERSION_INFO_is_le) VERSION_INFO_FUNCTION("VERSION_INFO_is_le", IS_LE)

#undef IS_LE

#undef IS_SAME

    void INCLUDE() {
        add_to_builtins("VERSION_INFO_version", VERSION_INFO_version);
        add_to_builtins("VERSION_INFO_is_same", VERSION_INFO_is_same);
        add_to_builtins("VERSION_INFO_is_gt", VERSION_INFO_is_gt);
        add_to_builtins("VERSION_INFO_is_lt", VERSION_INFO_is_lt);
        add_to_builtins("VERSION_INFO_is_ge", VERSION_INFO_is_ge);
        add_to_builtins("VERSION_INFO_is_le", VERSION_INFO_is_le);
    }
}

#endif //POURUPSCRIPTINTERP_LIB_VERSION_INFO_HPP
