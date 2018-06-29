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

template <typename OT, typename BT>
struct passthrough {
    using output_type = OT;
    using base_type = BT;

    passthrough()=default;
    explicit passthrough(base_type value) : value(output_type(std::move(value))) { }

    operator output_type () const {
        if(!value) {
            throw std::runtime_error("No passthrough value supplied");
        }

        return this->value.value();
    }

    std::optional<output_type> value;
};

namespace details {
template<typename LT>
struct back_inserter_adapter {
    using element_type = typename LT::value_type;
    using base_type = meta::derive_base_type_t<element_type>;
    using output_type = LT;
    static constexpr bool direct_from_yaml = true;

    static output_type create(std::string const &, YAML::Node const &config = {}) {
        output_type out;
        from_yamls<element_type>(config, std::back_inserter(out));
        return out;
    }

    static std::string get_name() {
        using namespace std::string_literals;
        return "list<"s + adapter<element_type>::get_name() + ">";
    }
};

template<typename ST>
struct inserter_adapter {
    using element_type = typename ST::value_type;
    using base_type = meta::derive_base_type_t<element_type>;
    using output_type = ST;
    static constexpr bool direct_from_yaml = true;

    static output_type create(std::string const &, YAML::Node const &config = {}) {
        output_type out;
        from_yamls<element_type>(config, std::inserter(out, out.end()));
        return out;
    }

    static std::string get_name() {
        using namespace std::string_literals;
        return "set<"s + adapter<element_type>::get_name() + ">";
    }
};

template<typename MT>
struct map_adapter {
    using output_type = MT;
    static constexpr bool direct_from_yaml = true;

    static output_type create(std::string const &, YAML::Node const &config = {}) {
        using key_type = typename MT::key_type;
        using mapped_type = typename MT::mapped_type;

        if (!config.IsMap()) {
            throw std::runtime_error("Must create map from map");
        }

        output_type out;
        std::map<std::string, YAML::Node> yamlMap = config.as<std::map<std::string, YAML::Node>>();
        for (auto const &entry : yamlMap) {
            key_type key = entry.first;
            mapped_type value = ::yadi::from_yaml<mapped_type>(entry.second);
            if (!out.insert(std::make_pair(std::move(key), std::move(value))).second) {
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

template<typename OT>
struct optional_adapter {
    using element_type = typename OT::value_type;
    using base_type = meta::derive_base_type_t<element_type>;
    using output_type = OT;
    static constexpr bool direct_from_yaml = ::yadi::adapter<element_type>::direct_from_yaml;

    static output_type create(std::string const &type, YAML::Node const &config = {}) {
        output_type out = ::yadi::create<element_type>(type, config);
        return out;
    }

    static std::string get_name() {
        using namespace std::string_literals;
        return "optional<"s + adapter<element_type>::get_name() + ">";
    }
};

template<typename PT>
struct passthrough_adapter {
    using element_type = typename PT::base_type;
    using base_type = meta::derive_base_type_t<typename PT::base_type>;
    using output_type = PT;
    using final_output_type = typename PT::output_type;
    static constexpr bool direct_from_yaml = ::yadi::adapter<base_type>::direct_from_yaml;

    static output_type create(std::string const &type, YAML::Node const &config = {}) {
        output_type out(::yadi::create<element_type>(type, config));
        return out;
    }

    static std::string get_name() {
        using namespace std::string_literals;
        return "passthrough<"s + adapter<final_output_type>::get_name() + ", " + adapter<element_type>::get_name() +
               ">";
    }
};
} // namespace details

template <typename ET>
struct adapter<std::list<ET>> : public details::back_inserter_adapter<std::list<ET>> {};

template <typename ET>
struct adapter<std::vector<ET>> : public details::back_inserter_adapter<std::vector<ET>> {};

template <typename ET>
struct adapter<std::set<ET>> : public details::inserter_adapter<std::set<ET>> {};

template <typename ET>
struct adapter<std::optional<ET>> : public details::optional_adapter<std::optional<ET>> {};

template <typename ET>
struct factory_traits<std::map<std::string, ET>> {
    using ptr_type = std::map<std::string, ET>;
    static const bool direct_from_yaml = true;
};

template <typename ET>
struct adapter<std::map<std::string, ET>> : public details::map_adapter<std::map<std::string, ET>> {};

template <typename ET>
struct factory_traits<std::unordered_map<std::string, ET>> {
    using ptr_type = std::unordered_map<std::string, ET>;
    static const bool direct_from_yaml = true;
};

template <typename ET>
struct adapter<std::unordered_map<std::string, ET>> : public details::map_adapter<std::unordered_map<std::string, ET>> {};

template <typename OT, typename BT>
struct adapter<passthrough<OT, BT>> : public details::passthrough_adapter<passthrough<OT, BT>> { };

}  // namespace yadi

#endif  // YADI_FACTORY_SPECIALIZATIONS_HPP
        // \endcond
