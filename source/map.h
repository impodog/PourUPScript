//
// Created by Dogs-Cute on 5/27/2023.
//

#ifndef PUPS_LIB_MAP_H
#define PUPS_LIB_MAP_H

#include "error.h"

namespace pups::library {
    using CatchRequirements = std::unordered_set<std::string>;

    class Map : public Object {
    protected:
        std::deque<ErrorPtr> m_errors;
        ObjectMap m_map;
        ObjectPtr m_base, m_return, m_temp;
        std::stack<ObjectPtr> m_memory_stack;
        std::string m_execute_string;
        std::queue<ObjectPtr *> m_unpacked;

        std::string m_debug_info;
        path m_path;

        using MapBarePtr = Map *;

        MapBarePtr m_sub_map = nullptr, m_parent_map = nullptr, m_restore_map = nullptr,
                m_deepest = this, m_global = this, m_upsearch_map = this;

        ObjectPtr &local_find(const Id &name);

        ObjectPtr &bare_find(const Id &name);

        ObjectPtr &single_find(const Id &name);

        ObjectPtr &staged_find(std::queue<std::string> &parts, Map *map = nullptr);

        void copy_signs_from(Map *map);

        void map_put(ObjectPtr &object, Map *map);

        void map_end_of_line();

        void unpack_array(ObjectPtr &object); // This is implemented in containers.cpp

        friend class Object;

        friend class Control;

    public:
        struct Signs {
            ObjectPtr break_sign = nullptr;
            bool else_sign = true;

            void set_break_sign(ObjectPtr object);
        } signs;

        explicit Map(path path);

        ~Map() override;

        Map(Map *parent_map, const path &path, bool allow_upsearch);

        Map(Map *parent_map, bool allow_upsearch);

        // When you need to switch parent map from far places(not under the local executing map),
        // pass the local map as restore_map
        Map(Map *parent_map, Map *restore_map, bool allow_upsearch);

        ObjectPtr put(ObjectPtr &object, Map *map) override;

        ObjectPtr &map_find(const Id &name, Map *map);

        ObjectPtr &source_find(const Id &name, Map *map) override;

        ObjectPtr end_of_line(Map *map) override;

        std::string type_name() const noexcept override;

        void throw_error(const ErrorPtr &error);

        void add_object(const Id &name, const ObjectPtr &object = pending);

        void set_object(const Id &name, const ObjectPtr &object = pending);

        void remove_object(const Id &name);

        ObjectPtr &get_return() noexcept;

        ObjectPtr &get_temp() noexcept;

        Map *get_parent() noexcept;

        Map *get_global() noexcept;

        Map *get_upsearch() noexcept;

        // Pass nullptr to unset the sub map, or a new map to set it.
        void set_child(Map *sub_map) noexcept;

        void restore() noexcept;

        ObjectPtr &add_to_memory_stack(const ObjectPtr &object);

        void report_errs();

        void inherit_errs(Map *map);

        void push_up_errs();

        void copy_objects_from(Map *map);

        void change_debug_info(std::string info);

        size_t count_depth() const noexcept;

        bool can_run() const noexcept;

        bool can_run_while() const noexcept;

        const ObjectMap &get_all_objects() const noexcept;

        bool catch_by(CatchRequirements &required);

        bool is_child_of(Map *map);

        const path &get_path();
    };

    // This saves the number of errors reported. Can be freely set to 0.
    extern size_t err_count;
}

#endif //PUPS_LIB_MAP_H
