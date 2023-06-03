//
// Created by Dogs-Cute on 5/27/2023.
//
#include "source/control.h"
#include "source/builtins/builtins.h"

using namespace pups::library;

int main() {
    Constants constants("test/main.con");
    builtins::init(constants);
    Control control(path{"test/main.pups"}, constants);
    control.run();
    return 0;
}