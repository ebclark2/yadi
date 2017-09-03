//
// Created by Ed Clark on 8/29/17.
//

#ifndef YADI_HELP_HPP
#define YADI_HELP_HPP

#include "demangle.hpp"

#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <vector>

namespace yadi {

struct yadi_help_fetcher {
    struct concept {
        virtual std::string getHelp(std::string const& type) const = 0;
        virtual std::vector<std::string> getTypes() const = 0;
        virtual std::unique_ptr<concept> clone() const = 0;
    };

    template <typename T>
    struct model : public concept {
        model(T const& types) : types(types) {}

        std::string getHelp(std::string const& type) const override {
            auto types_iter = this->types.find(type);
            if (types_iter == this->types.end()) {
                throw std::runtime_error("Type \"" + type + "\" not found");
            }

            return types_iter->second.help;
        }

        std::vector<std::string> getTypes() const override {
            std::vector<std::string> ret;
            for (auto const& entry : this->types) {
                ret.push_back(entry.first);
            }

            return ret;
        }

        std::unique_ptr<concept> clone() const override {
            std::unique_ptr<concept> copy(new model(this->types));
            return copy;
        }

        T const& types;
    };

    yadi_help_fetcher();

    yadi_help_fetcher(yadi_help_fetcher const& other);

    template <typename Y>
    yadi_help_fetcher(Y const& types) : impl(new model<Y>(types)) {}

    yadi_help_fetcher& operator=(yadi_help_fetcher const& other);

    inline std::string get_help(std::string const& type) const { return this->impl->getHelp(type); }
    inline std::vector<std::string> get_types() const { return this->impl->getTypes(); }

   private:
    std::unique_ptr<concept> impl;
};

struct yadi_help {
    using help_store = std::map<std::string, yadi_help_fetcher>;
    using name_store = std::map<std::type_index, std::string>;

    template <typename BT, typename TS>
    static void register_factory(std::string name, TS const& types) {
        mut_helps()[name] = types;
        mut_names()[std::type_index(typeid(BT))] = name;
    }

    template <typename BT>
    static bool has_name() {
        auto const& name_iter = names().find(std::type_index(typeid(BT)));
        return name_iter != names().end();
    }

    template <typename BT>
    static std::string get_name() {
        auto const& name_iter = names().find(std::type_index(typeid(BT)));
        if (name_iter != names().end()) {
            return name_iter->second;
        }

        return demangle_type<BT>();
    }

    static help_store const& helps();
    static name_store const& names();

   private:
    static help_store& mut_helps();
    static name_store& mut_names();
};

}  // namespace yadi

#endif  // YADI_HELP_HPP
