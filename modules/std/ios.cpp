//
// Created by impodog on 7/1/2023.
//

#include "ios.h"
#include "cstdio"

namespace pups::modules::ios {
    using path_t = std::filesystem::path;

    class File;

    using FileFuncCore = std::function<ObjectPtr(File &, FunctionArgs &, Map *)>;
    using FileFuncMap = library::IdMap<FileFuncCore>;

    extern const FileFuncMap file_functions;

    class File : public HasMethods {
    public:
        path_t m_path;
        std::fstream m_file;

        File(path_t path, Map *map) : m_path(std::move(path)), m_file(m_path) {
            if (!m_file.is_open())
                map->throw_error(std::make_shared<library::FileNotFoundError>("When creating file instance, "
                                                                              "file \"" + m_path.string() +
                                                                              "\" does not exist."));
        }

        ObjectPtr put(ObjectPtr &object, Map *map) override {
            map->throw_error(std::make_shared<library::ArgumentError>("File object does not allow putting."));
            return nullptr;
        }

        FunctionCore get_method(const Id &name) override {
            const auto &func = file_functions.at(name);
            return [this, &func](FunctionArgs &args, Map *map) -> ObjectPtr {
                return func(*this, args, map);
            };
        }

        std::string type_name() const noexcept override {
            return "file";
        }
    };

    ObjectPtr file_read(File &file, FunctionArgs &args, Map *map) {
        std::string buf;
        while (!args.empty()) {
            if (file.m_file.eof())
                *args.front() = pending;
            file.m_file >> buf;
            *args.front() = std::make_shared<strings::String>(buf);
        }
        return pending;
    }

    ObjectPtr file_readall(File &file, FunctionArgs &args, Map *map) {
        if (!args.empty())
            map->throw_error(
                    std::make_shared<library::ArgumentError>("file.readall requires no arguments. Arguments ignored."));
        std::string result((std::istreambuf_iterator<char>(file.m_file)),
                           std::istreambuf_iterator<char>());
        return std::make_shared<strings::String>(result);
    }

    ObjectPtr file_write(File &file, FunctionArgs &args, Map *map) {
        std::string content = strings::str_of(args);
        file.m_file << content;
        return pending;
    }

    ObjectPtr file_flush(File &file, FunctionArgs &args, Map *map) {
        if (!args.empty())
            map->throw_error(
                    std::make_shared<library::ArgumentError>("file.flush requires no arguments. Arguments ignored."));
        file.m_file.flush();
        return pending;
    }

    ObjectPtr file_endl(File &file, FunctionArgs &args, Map *map) {
        if (!args.empty())
            map->throw_error(
                    std::make_shared<library::ArgumentError>("file.endl requires no arguments. Arguments ignored."));
        file.m_file << std::endl;
        return pending;
    }

    ObjectPtr file_reset(File &file, FunctionArgs &args, Map *map) {
        if (!args.empty())
            map->throw_error(
                    std::make_shared<library::ArgumentError>("file.reset requires no arguments. Arguments ignored."));
        file.m_file.close();
        file.m_file.open(file.m_path, std::ios::out);
        file.m_file.close();
        file.m_file.open(file.m_path);
        return pending;
    }

    ObjectPtr file_close(File &file, FunctionArgs &args, Map *map) {
        if (!args.empty())
            map->throw_error(
                    std::make_shared<library::ArgumentError>("file.close requires no arguments. Arguments ignored."));
        file.m_file.close();
        return pending;
    }

    class Path;

    using PathFuncCore = std::function<ObjectPtr(Path &, FunctionArgs &, Map *)>;
    using PathFuncMap = library::IdMap<PathFuncCore>;

    extern const PathFuncMap path_functions;

    class Path : public HasMethods {
    public:
        path_t m_path;

        explicit Path(path_t path) : m_path(std::move(path)) {}

        ObjectPtr put(ObjectPtr &object, Map *map) override {
            map->throw_error(std::make_shared<library::ArgumentError>("Path object does not allow putting."));
            return nullptr;
        }

        FunctionCore get_method(const Id &name) override {
            const auto &func = path_functions.at(name);
            return [this, &func](FunctionArgs &args, Map *map) -> ObjectPtr {
                return func(*this, args, map);
            };
        }

        std::string type_name() const noexcept override {
            return "path";
        }

        std::string str() const noexcept override {
            return "\"" + m_path.string() + "\"";
        }

        size_t hash() const noexcept override {
            return std::hash<std::string>()(std::filesystem::absolute(m_path).string());
        }

        size_t equal(const ObjectPtr &object) const noexcept override {
            auto ptr = cast<Path>(object);
            return ptr && m_path == ptr->m_path;
        }
    };

    ObjectPtr path_add(Path &path, FunctionArgs &args, Map *map) {
        path_t result = path.m_path;
        while (!args.empty()) {
            auto ptr = cast<strings::String>(*args.front());
            if (ptr)
                result.append(ptr->data());
            else
                map->throw_error(std::make_shared<library::TypeError>(
                        "path.add requires string arguments, but " + args.front()->get()->repr() + " is got."));
            args.pop_front();
        }
        return std::make_shared<Path>(result);
    }

    ObjectPtr path_get_str(Path &path, FunctionArgs &args, Map *map) {
        if (!args.empty())
            map->throw_error(
                    std::make_shared<library::ArgumentError>("path.get_str requires no arguments. Arguments ignored."));
        return std::make_shared<strings::String>(path.m_path.string());
    }

    ObjectPtr path_stem(Path &path, FunctionArgs &args, Map *map) {
        if (!args.empty())
            map->throw_error(
                    std::make_shared<library::ArgumentError>("path.stem requires no arguments. Arguments ignored."));
        return std::make_shared<Path>(path.m_path.stem());
    }

    ObjectPtr path_extn(Path &path, FunctionArgs &args, Map *map) {
        if (!args.empty())
            map->throw_error(
                    std::make_shared<library::ArgumentError>("path.extn requires no arguments. Arguments ignored."));
        return std::make_shared<Path>(path.m_path.extension());
    }

    ObjectPtr path_abs(Path &path, FunctionArgs &args, Map *map) {
        if (!args.empty())
            map->throw_error(
                    std::make_shared<library::ArgumentError>("path.abs requires no arguments. Arguments ignored."));
        return std::make_shared<Path>(std::filesystem::absolute(path.m_path));
    }

    ObjectPtr path_parent(Path &path, FunctionArgs &args, Map *map) {
        if (!args.empty())
            map->throw_error(
                    std::make_shared<library::ArgumentError>("path.parent requires no arguments. Arguments ignored."));
        return std::make_shared<Path>(path.m_path.parent_path());
    }

    ObjectPtr path_exists(Path &path, FunctionArgs &args, Map *map) {
        if (!args.empty())
            map->throw_error(
                    std::make_shared<library::ArgumentError>("path.exists requires no arguments. Arguments ignored."));
        return std::make_shared<numbers::BoolType>(std::filesystem::exists(path.m_path));
    }

    ObjectPtr path_is_file(Path &path, FunctionArgs &args, Map *map) {
        if (!args.empty())
            map->throw_error(
                    std::make_shared<library::ArgumentError>("path.is_file requires no arguments. Arguments ignored."));
        return std::make_shared<numbers::BoolType>(std::filesystem::is_regular_file(path.m_path));
    }

    ObjectPtr path_is_dir(Path &path, FunctionArgs &args, Map *map) {
        if (!args.empty())
            map->throw_error(
                    std::make_shared<library::ArgumentError>("path.is_dir requires no arguments. Arguments ignored."));
        return std::make_shared<numbers::BoolType>(std::filesystem::is_directory(path.m_path));
    }

    ObjectPtr path_list_dir(Path &path, FunctionArgs &args, Map *map) {
        if (!args.empty())
            map->throw_error(
                    std::make_shared<library::ArgumentError>(
                            "path.list_dir requires no arguments. Arguments ignored."));
        if (std::filesystem::is_directory(path.m_path)) {
            std::vector<ObjectPtr> result;
            for (auto const &entry: std::filesystem::directory_iterator(path.m_path))
                result.push_back(std::make_shared<Path>(entry.path()));
            return std::make_shared<containers::Array>(result);
        } else
            map->throw_error(std::make_shared<library::ValueError>(
                    "path.list_dir requires a directory to list, " "instead of " + path.str() + "."));
        return pending;
    }

    template<bool staged>
    void list_recur(std::vector<ObjectPtr> &result, const path_t &path) {
        if (std::filesystem::is_directory(path)) {
            if constexpr (staged) {
                auto arr = std::make_shared<containers::Array>();
                for (auto const &entry: std::filesystem::directory_iterator(path))
                    list_recur<staged>(arr->data, entry.path());
                result.push_back(arr);
            } else {
                for (auto const &entry: std::filesystem::directory_iterator(path))
                    list_recur<staged>(result, entry.path());
            }
        } else
            result.push_back(std::make_shared<Path>(path));
    }

    template<bool staged>
    ObjectPtr path_list_recur(Path &path, FunctionArgs &args, Map *map) {
        if (!args.empty())
            map->throw_error(
                    std::make_shared<library::ArgumentError>(
                            "path.list_recur requires no arguments. Arguments ignored."));
        std::vector<ObjectPtr> result;
        list_recur<staged>(result, path.m_path);
        if constexpr (staged)
            return result.front();
        else
            return std::make_shared<containers::Array>(result);
    }

    ObjectPtr path_create(Path &path, FunctionArgs &args, Map *map) {
        if (!args.empty())
            map->throw_error(
                    std::make_shared<library::ArgumentError>(
                            "path.create requires no arguments. Arguments ignored."));
        std::ofstream out{path.m_path};
        return pending;
    }

    class FileInit : public Function {
    public:
        FileInit() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
            if (args.size() != 1)
                map->throw_error(
                        std::make_shared<library::ArgumentError>("File initialization requires one only argument."));
            else {
                {
                    auto ptr = cast<strings::String>(*args.front());
                    if (ptr)
                        return std::make_shared<File>(path_t{ptr->data()}, map);
                }
                {
                    auto ptr = cast<Path>(*args.front());
                    if (ptr)
                        return std::make_shared<File>(ptr->m_path, map);
                }
                map->throw_error(std::make_shared<library::TypeError>(
                        "File initialization should get either a string or a path."));
            }
            return pending;
        }) {}
    };

    class PathInit : public Function {
    public:
        PathInit() : Function([](FunctionArgs &args, Map *map) -> ObjectPtr {
            if (args.size() != 1)
                map->throw_error(
                        std::make_shared<library::ArgumentError>("Path initialization requires one only argument."));
            else {
                auto ptr = cast<strings::String>(*args.front());
                if (ptr)
                    return std::make_shared<Path>(path_t{ptr->data()});
                else
                    map->throw_error(std::make_shared<library::TypeError>(
                            "Path initialization should get a string."));
            }
            return pending;
        }) {}
    };

    Id id_ios{"", "ios"};
    Id id_fileInit{"", "open"};
    Id id_fileRead{"", "read"}, id_fileReadall{"", "readall"},
            id_fileWrite{"", "write"}, id_fileFlush{"", "flush"}, id_fileEndl{"", "endl"},
            id_fileReset{"", "reset"}, id_fileClose{"", "close"};
    Id id_pathInit{"", "path"};
    Id id_pathAdd{"", "add"}, id_pathGetStr{"", "get_str"}, id_pathStem{"", "stem"}, id_pathExtn{"", "extn"},
            id_pathAbs{"", "abs"}, id_pathParent{"", "parent"}, id_pathExists{"", "exists"},
            id_pathIsFile{"", "is_file"}, id_pathIsDir{"", "id_dir"},
            id_pathListDir{"", "list_dir"}, id_pathListRecur{"", "list_recur"}, id_pathListFiles{"", "list_files"},
            id_pathCreate{"", "create"};

    const FileFuncMap file_functions = {
            {id_fileRead,    file_read},
            {id_fileReadall, file_readall},
            {id_fileWrite,   file_write},
            {id_fileFlush,   file_flush},
            {id_fileEndl,    file_endl},
            {id_fileReset,   file_reset},
            {id_fileClose,   file_close}
    };
    const PathFuncMap path_functions = {
            {id_pathAdd,       path_add},
            {id_pathGetStr,    path_get_str},
            {id_pathStem,      path_stem},
            {id_pathExtn,      path_extn},
            {id_pathAbs,       path_abs},
            {id_pathParent,    path_parent},
            {id_pathExists,    path_exists},
            {id_pathIsFile,    path_is_file},
            {id_pathIsDir,     path_is_dir},
            {id_pathListDir,   path_list_dir},
            {id_pathListRecur, path_list_recur<true>},
            {id_pathListFiles, path_list_recur<false>},
            {id_pathCreate,    path_create}
    };

    void init(Constants &constants) {
        auto &ios = constants.new_sub_const(id_ios);
        ios.add(id_fileInit, std::make_shared<FileInit>());
        ios.add(id_pathInit, std::make_shared<PathInit>());
    }
}