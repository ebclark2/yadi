//
// Created by Ed Clark on 8/27/17.
//

#ifndef YADI_YADI_HELP_HPP
#define YADI_YADI_HELP_HPP

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace yadi {

struct yadi_help_fetcher {
    struct concept {
        virtual std::string getHelp(std::string const& type) const = 0;
        virtual std::vector<std::string> getTypes() const = 0;
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

        T const& types;
    };

    template <typename Y>
    yadi_help_fetch(Y const& types) : impl(new model<Y>(types)) {}

    std::string getHelp(std::string const& type) const { return this->impl->getHelp(type); }
    std::vector<std::string> getTypes() const { return this->impl->getTypes(); }

    std::unique_ptr<concept> impl;
};

class yadi_help {
    using help_store = std::map<std::string, yadi_help_fetcher>;

    template <typename base_t>
    static void register_factory(std::string name) {
        helps()[name] = factory<base_t>::types();
    }

    static help_store& helps() {
        static help_store HELPS;
        return HELPS;
    }
};

}  // namespace yadi

#endif  // YADI_YADI_HELP_HPP
