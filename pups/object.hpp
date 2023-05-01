//
// Created by Dogs-Cute on 4/22/2023.
//

#ifndef POURUPSCRIPT_BASE_HPP
#define POURUPSCRIPT_BASE_HPP

#endif //POURUPSCRIPT_BASE_HPP

#include "report.hpp"

namespace PUPS {
    class Scope;

    struct EvalResult {
        enum ResultType {
            type_int,
            type_float,
            type_str,
            type_byte,
            type_schar
        } type;
        union ResultUnion {
            long long r_int;
            double r_float;
            const std::string *r_str;
            unsigned char r_byte;
            signed char r_schar;
        } result;
    };

    class ObjectBase {
    protected:
        using Cnt = size_t;
        static Cnt count;
        using CountSearch = std::vector<Cnt>;

        union Access {
            int i;
        };
    public:
        using ObjectPtr = std::shared_ptr<ObjectBase>;
        const Cnt cnt = count++;

        virtual void put(const Token &token, Report &report) = 0;

        virtual ObjectPtr ends(PUPS::Scope *scope, PUPS::Report &report) = 0;

        virtual void exit(Report &report) = 0;

        [[nodiscard]] virtual bool can_delete() const noexcept {
            return true;
        }

        [[nodiscard]] virtual Cnt type() const noexcept {
            return 0;
        }

        virtual ~ObjectBase() = default;

        [[nodiscard]] virtual std::string to_string() const noexcept {
            return "<&Object=" + std::to_string(PtrToUlong(this)) + " Code=" + std::to_string(cnt) + ">";
        }

        [[nodiscard]] virtual std::string to_repr() const noexcept {
            return this->to_string();
        }

        [[nodiscard]] virtual constexpr bool is_scope() const noexcept {
            return false;
        }

        [[nodiscard]] virtual constexpr bool is_type() const noexcept {
            return false;
        }

        [[nodiscard]] virtual constexpr bool is_instance() const noexcept {
            return false;
        }

        virtual EvalResult getv() const {
            throw std::runtime_error("cannot evaluate object " + to_string());
        }

        bool operator==(const ObjectBase &object) const noexcept {
            return cnt == object.cnt;
        }
    };

    using ObjectPtr = ObjectBase::ObjectPtr;

    size_t ObjectBase::count = 1;

    extern ObjectPtr null_obj;

    class Null : public ObjectBase {
    public:
        Null() = default;

        void put(const Token &token, Report &report) override {
            report.report(Report_NullErr,
                          "Cannot put token \"" + std::string(token) + "\" (or anything else) into null.");
        }

        ObjectPtr ends(PUPS::Scope *scope, PUPS::Report &report) override {
            return null_obj;
        }

        void exit(Report &report) override {
            //intended to do nothing
        }

        [[nodiscard]] bool can_delete() const noexcept override {
            return false;
        }

        [[nodiscard]] std::string to_string() const noexcept override {
            return "NULL";
        }
    };

    ObjectPtr null_obj = ObjectPtr{new Null};

    inline bool is_null_or_null(const ObjectPtr &object) noexcept {
        return object == nullptr || object == null_obj;
    }

    FN_ALIAS(is_non, is_null_or_null)

    inline std::string get_ptr_string(const ObjectPtr &ptr) noexcept {
        if (ptr == nullptr) return "<UNINITIALIZED>";
        return ptr->to_string();
    }
}