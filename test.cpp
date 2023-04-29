//
// Created by Dogs-Cute on 4/22/2023.
//

#include "pups.h"

using namespace std;
using namespace PUPS;

int main() {
    Report::output = &std::cout;
    Keywords keywords;
    Scripter scripter("test/test.pups", keywords);
    while (scripter.forward()) {
        scripter.report_all();
    }
    return 0;
}