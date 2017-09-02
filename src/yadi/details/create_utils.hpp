//
// Created by Ed Clark on 8/29/17.
//

#ifndef YADI_CREATE_UTILS_HPP
#define YADI_CREATE_UTILS_HPP

#include "factory.hpp"
#include "type_utils.hpp"

#include <yaml-cpp/yaml.h>

namespace yadi {

std::string const& type_by_value_key();

/**
 * @brief Same as factory<BT>::create(type, config)
 * @tparam BT
 * @param type
 * @param config
 * @return
 */
template <typename OT>
OT create(std::string const& type, YAML::Node const& config = {});

/**
 * @brief Pulls type and config from YAML.  This function is especially usefil when loading
 * nested types from YAML configuration.  If factory_config is a scalar string it will be used
 * as type.  If factory_config is a map then "type" and "config" keys will be pulled from it and
 * used as such.
 * @param factory_config
 * @return
 */
template <typename OT>
OT from_yaml(YAML::Node const& factory_config);

// TODO COMMENT
template <typename BT>
ptr_type_t<BT> from_yaml_base(YAML::Node const& config = {}) {
    return from_yaml<ptr_type_t<BT>>(config);
}

/**
 * @brief Populate output iterator from sequence of factory configs (anything from_yaml accepts).
 * @tparam BT base type
 * @tparam OI Output iterator
 * @param factory_configs
 * @param out
 */
template <typename OT, typename OI>
void from_yamls(YAML::Node const& factory_configs, OI out);

template <typename BT, typename OI>
void from_yamls_base(YAML::Node const& factory_configs, OI out) {
    from_yamls<ptr_type_t<BT>>(factory_configs, out);
};

/**
 * @brief Populate out from factory config.  The factory type is derived from ptr_type.
 * @tparam OT output type
 * @param out
 * @param factory_config
 */
template <typename OT>
void parse(OT& out, YAML::Node const& factory_config);

template <typename FT, typename OT = ptr_type_t<derive_base_type_t<FT>>>
struct adapter {
    using base_type = derive_base_type_t<FT>;
    using output_type = OT;

    // TODO add some function to show factory return type
    static_assert(std::is_convertible<ptr_type_t<base_type>, OT>::value,
                  "Unable to convert factory return type to desired output type in ");

    static output_type create(std::string const& type, YAML::Node const& config = {}) {
        return factory<base_type>::create(type, config);
    }
};

// ################# IMPL #####################
template <typename OT>
OT create(std::string const& type, YAML::Node const& config) {
    return adapter<OT>::create(type, config);
}

template <typename OT>
OT from_yaml(YAML::Node const& factory_config) {
    using BT = derive_base_type_t<OT>;
    using DOT = OT;  // derive_output_type_t<OT>;
    if (adapter<OT, OT>::direct_from_yaml) {
        return adapter<BT, DOT>::create(type_by_value_key(), factory_config);
    }

    if (!factory_config.IsDefined()) {
        throw std::runtime_error("Factory config not defined");
    }

    if (factory_config.IsScalar()) {
        std::string type = factory_config.as<std::string>("");
        if (type.empty()) {
            throw std::runtime_error("Factory config scalar not valid");
        }

        return adapter<BT, DOT>::create(type);
    }

    if (factory_config.IsMap()) {
        YAML::Node typeNode = factory_config["type"];
        if (!typeNode.IsDefined()) {
            throw std::runtime_error("Factory config type not defined");
        }
        std::string type = typeNode.as<std::string>("");
        if (type.empty()) {
            throw std::runtime_error("Factory config type not valid");
        }

        YAML::Node configNode = factory_config["config"];
        return adapter<BT, DOT>::create(type, configNode);
    }

    throw std::runtime_error("Factory config not valid, YAML must be scalar string or map");
}

template <typename OT, typename OI>
void from_yamls(YAML::Node const& factory_configs, OI out) {
    if (!factory_configs.IsDefined()) {
        throw std::runtime_error("From YAML factory configs not defined");
    }
    // If it's not a sequence then parse single
    if (!factory_configs.IsSequence()) {
        *out = from_yaml<OT>(factory_configs);
        ++out;
        return;
    }

    // A sequence!
    for (YAML::Node const& entry : factory_configs) {
        *out = from_yaml<OT>(entry);
        ++out;
    }
}

template <typename OT>
void parse(OT& out, YAML::Node const& factory_config) {
    out = from_yaml<OT>(factory_config);
}

template <typename FT>
struct adapter<FT, ptr_type_t<derive_base_type_t<FT>>> {
    using base_type = derive_base_type_t<FT>;
    using output_type = ptr_type_t<derive_base_type_t<FT>>;
    static bool const direct_from_yaml = factory_traits<base_type>::direct_from_yaml;

    static output_type create(std::string const& type, YAML::Node const& config = {}) {
        return factory<base_type>::create(type, config);
    }
};

}  // namespace yadi

#endif  // YADI_CREATE_UTILS_HPP
