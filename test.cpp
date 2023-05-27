//
// Created by Dogs-Cute on 5/27/2023.
//
#include "source/control.h"

using namespace pups::library;

int main() {
    Control control(path{"test/main.pups"});
    while (!control.next_line()) {

    }
    return 0;
}