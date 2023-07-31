//
// Created by impodog on 7/31/2023.
//

#ifndef CPP_PUPPY_EXPORT_H
#define CPP_PUPPY_EXPORT_H

#include <filesystem>

void script(std::filesystem::path file, const std::filesystem::path &output, bool keep_temp, bool debug);

#endif //CPP_PUPPY_EXPORT_H
