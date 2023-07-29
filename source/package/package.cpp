//
// Created by impodog on 7/27/2023.
//

#include "package.h"
#include <vector>

/* This part defines macros for loading dynamic libraries */
#ifdef __WIN32

#include <windows.h>

#define LIB_HANDLE HINSTANCE
#define LIB_LOAD(name) LoadLibrary(name)
#define LIB_GET(lib, name) GetProcAddress(lib, name)
#define LIB_CLOSE(lib) FreeLibrary(lib)


#else

#include <dlfcn.h>
#define LIB_HANDLE void *
#define LIB_LOAD(name) dlopen(name, RTLD_LAZY)
#define LIB_GET(lib, name) dlsym(lib, name)
#define LIB_CLOSE(lib) dlclose(lib)

#endif

/*This part defines different dynamic library extensions*/
#ifdef __WIN32
#define LIB_EXT ".dll"
#elifdef __APPLE__
#define LIB_EXT ".dylib"
#else
#define LIB_EXT ".so"
#endif

namespace pups::library::package {
    std::vector<LIB_HANDLE> loaded_libs;

    std::string get_init_function_name(const std::string &name) {
        return "PUPS_Init_" + name;
    }

    path combine_path(const path &parent, const std::string &name) {
        return parent / ("lib" + name + LIB_EXT);
    }

    LIB_HANDLE load(const path &file) {
        LIB_HANDLE lib = LIB_LOAD(file.string().c_str());
        if (lib == nullptr) {
            throw std::runtime_error("Cannot load library " + file.string());
        }
        loaded_libs.push_back(lib);
        return lib;
    }

    void pkg_load(const path &parent, const std::string &name, PackageHandler &handler) {
        auto file = combine_path(parent, name);
        LIB_HANDLE lib = load(file);
        std::string init_function_name = get_init_function_name(name);
        auto init = (PUPS_InitFunction) LIB_GET(lib, init_function_name.c_str());
        if (init == nullptr) {
            throw std::runtime_error(
                    "Cannot find init function \"" + init_function_name + "\" in library " + file.string());
        }
        init(handler);
    }

    void pkg_load_export(const path &parent, const std::string &name, Map *map) {
        PackageHandler handler;
        pkg_load(parent, name, handler);
        handler.export_to(map);
        map->set_child(nullptr);
    }

    ObjectPtr import_lib(FunctionArgs &args, Map *map) {
        if (args.size() != 2) {
            map->throw_error(std::make_shared<ArgumentError>("import_lib requires two only arguments."));
        } else {
            auto first = cast<strings::String>(*args.front());
            args.pop_front();
            auto second = cast<strings::String>(*args.front());
            args.pop_front();
            if (first && second) {
                pkg_load_export(map->get_path() / first->data(), second->data(), map);
            } else {
                map->throw_error(std::make_shared<TypeError>("import_lib requires two string arguments."));
            }
        }
        return pending;
    }

    Id id_importLib{"", "import_lib"};

    void init(Constants &constants) {
        constants.add(id_importLib, std::make_shared<Function>(import_lib));
    }

    void quit() {
        for (auto &lib: loaded_libs) {
            LIB_CLOSE(lib);
        }
        loaded_libs.clear();
    }
}