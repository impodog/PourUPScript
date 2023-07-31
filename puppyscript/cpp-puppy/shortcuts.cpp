//
// Created by impodog on 7/31/2023.
//
#include "processor.h"

std::regex RE_PAIR("(" WORD ")\\s*::\\s*(" WORD ")"),
        RE_RANGE_STEP("(" WORD ")\\s*->\\s*(" WORD ")\\s*\\|\\s*(" WORD ")"),
        RE_RANGE_NO_STEP("(" WORD ")\\s*->\\s*(" WORD ")");

class Shortcuts {
    std::string content;

    /* DEPRECATED
    void scan_pairs() {
        content = std::regex_replace(content, RE_PAIR, "(pair $1 $2)");
    }

    void scan_ranges() {
        content = std::regex_replace(content, RE_RANGE_STEP, "(range $1 $2 $3)");
        content = std::regex_replace(content, RE_RANGE_NO_STEP, "(range $1 $2)");
    }
     */

public:
    explicit Shortcuts(const path &file) {
        content = read(file);
    }

    std::string work(const std::string &output_name) {

        std::string output = output_name + ".shortcuts.puppy";
        write(output, content);
        return output;
    }
};

PROCESSOR(shortcuts) {
    Shortcuts shortcuts(file);
    return shortcuts.work(output_name);
}