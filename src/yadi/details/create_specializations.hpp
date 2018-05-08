//
// Created by Ed Clark on 8/29/17.
//
// \cond DEV_DOCS
#ifndef YADI_FACTORY_SPECIALIZATIONS_HPP
#define YADI_FACTORY_SPECIALIZATIONS_HPP

#include "create_utils.hpp"
#include "factory.hpp"

#include <iterator>
#include <list>
#include <map>
#include <optional>
#include <unordered_map>
#include <vector>

namespace yadi {

template <typename FT>
struct adapter<FT, ptr_type_t<meta::derive_base_type_t<FT>>> {
    using base_type = meta::derive_base_type_t<FT>;
    using output_type = ptr_type_t<meta::derive_base_type_t<FT>>;
    static constexpr bool direct_from_yaml = factory_traits<base_type>::direct_from_yaml;

    static output_type create(std::string const& type, YAML::Node const& config = {}) {
        return factory<base_type>::create(type, config);
    }

    static std::string get_name() { return yadi_help::get_name<base_type>(); }
};

template <typename LT, typename ET>
struct back_inserter_adapter {
    using base_type = meta::derive_base_type_t<ET>;
    using output_type = LT;
    static constexpr bool direct_from_yaml = true;

    static output_type create(std::string const&, YAML::Node const& config = {}) {
        output_type out;
        from_yamls<ET>(config, std::back_inserter(out));
        return out;
    }

    static std::string get_name() {
        using namespace std::string_literals;
        return "list<"s + adapter<ET>::get_name() + ">";
    }
};

template <typename ST, typename ET>
struct inserter_adapter {
    using base_type = meta::derive_base_type_t<ET>;
    using output_type = ST;
    static constexpr bool direct_from_yaml = true;

    static output_type create(std::string const&, YAML::Node const& config = {}) {
        output_type out;
        from_yamls<ET>(config, std::inserter(out, out.end()));
        return out;
    }

    static std::string get_name() {
        using namespace std::string_literals;
        return "set<"s + adapter<ET>::get_name() + ">";
    }
};

template <typename MT>
struct map_adapter {
    using output_type = MT;
    static constexpr bool direct_from_yaml = true;

    static output_type create(std::string const&, YAML::Node const& config = {}) {
        using key_type = typename MT::key_type;
        using mapped_type = typename MT::mapped_type;

        if(!config.IsMap()) {
            throw std::runtime_error("Must create map from map");
        }

        output_type out;
        std::map<std::string, YAML::Node> yamlMap = config.as<std::map<std::string, YAML::Node>>();
        for(auto const& entry : yamlMap) {
            key_type key = entry.first;
            mapped_type value = ::yadi::from_yaml<mapped_type>(entry.second);
            if(!out.insert(std::make_pair(std::move(key), std::move(value))).second) {
                throw std::runtime_error("Map must contiain unique keys");
            }
        }
        return out;
    }

    static std::string get_name() {
        using namespace std::string_literals;
        return "map<"s + ::yadi::demangle_type<typename MT::value_type>() + ">";
    }
};

template <typename OT, typename ET>
struct optional_adapter {
    using base_type = meta::derive_base_type_t<ET>;
    using output_type = OT;
    static constexpr bool direct_from_yaml = ::yadi::adapter<ET>::direct_from_yaml;

    static output_type create(std::string const& type, YAML::Node const& config = {}) {
        output_type out = ::yadi::create<ET>(type, config);
        return out;
    }

    static std::string get_name() {
        using namespace std::string_literals;
        return "optional<"s + adapter<ET>::get_name() + ">";
    }
};

template <typename ET>
struct adapter<std::list<ET>, std::list<ET>> : public back_inserter_adapter<std::list<ET>, ET> {};

template <typename ET>
struct adapter<std::vector<ET>, std::vector<ET>> : public back_inserter_adapter<std::vector<ET>, ET> {};

template <typename ET>
struct adapter<std::set<ET>, std::set<ET>> : public inserter_adapter<std::set<ET>, ET> {};

template <typename ET>
struct adapter<std::optional<ET>, std::optional<ET>> : public optional_adapter<std::optional<ET>, ET> {};

template <typename ET>
struct factory_traits<std::map<std::string, ET>> {
    using ptr_type = std::map<std::string, ET>;
    static const bool direct_from_yaml = true;
};

template <typename ET>
struct adapter<std::map<std::string, ET>> : public map_adapter<std::map<std::string, ET>> {};

template <typename ET>
struct factory_traits<std::unordered_map<std::string, ET>> {
    using ptr_type = std::unordered_map<std::string, ET>;
    static const bool direct_from_yaml = true;
};

template <typename ET>
struct adapter<std::unordered_map<std::string, ET>> : public map_adapter<std::unordered_map<std::string, ET>> {};

}  // namespace yadi

#endif  // YADI_FACTORY_SPECIALIZATIONS_HPP
        // \endcond
