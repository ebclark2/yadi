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
template <typename BT>
ptr_type_t<BT> create(std::string const& type, YAML::Node const& config = {});

/**
 * @brief Pulls type and config from YAML.  This function is especially usefil when loading
 * nested types from YAML configuration.  If factory_config is a scalar string it will be used
 * as type.  If factory_config is a map then "type" and "config" keys will be pulled from it and
 * used as such.
 * @param factory_config
 * @return
 */
template <typename BT>
ptr_type_t<BT> from_yaml(YAML::Node const& factory_config);

/**
 * @brief Populate output iterator from sequence of factory configs (anything from_yaml accepts).
 * @tparam BT base type
 * @tparam OI Output iterator
 * @param factory_configs
 * @param out
 */
template <typename BT, typename OI>
void from_yamls(YAML::Node const& factory_configs, OI out);

/**
 * @brief Populate out from factory config.  The factory type is derived from ptr_type.
 * @tparam PT pointer type
 * @param out
 * @param factory_config
 */
template <typename PT>
void parse(PT& out, YAML::Node const& factory_config);

// ################# IMPL #####################
template <typename BT>
ptr_type_t<BT> create(std::string const& type, YAML::Node const& config) {
    return factory<BT>::create(type, config);
}

template <typename BT>
ptr_type_t<BT> from_yaml(YAML::Node const& factory_config) {
    if (factory_traits<BT>::direct_from_yaml) {
        return factory<BT>::create(type_by_value_key(), factory_config);
    }

    if (!factory_config.IsDefined()) {
        throw std::runtime_error("Factory config not defined");
    }

    if (factory_config.IsScalar()) {
        std::string type = factory_config.as<std::string>("");
        if (type.empty()) {
            throw std::runtime_error("Factory config scalar not valid");
        }

        return factory<BT>::create(type);
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
        return factory<BT>::create(type, configNode);
    }

    throw std::runtime_error("Factory config not valid, YAML must be scalar string or map");
}

template <typename BT, typename OI>
void from_yamls(YAML::Node const& factory_configs, OI out) {
    if (!factory_configs.IsDefined()) {
        throw std::runtime_error("From YAML factory configs not defined");
    }
    // If it's not a sequence then parse single
    if (!factory_configs.IsSequence()) {
        *out = from_yaml<BT>(factory_configs);
        ++out;
        return;
    }

    // A sequence!
    for (YAML::Node const& entry : factory_configs) {
        *out = from_yaml<BT>(entry);
        ++out;
    }
}

template <typename PT>
void parse(PT& out, YAML::Node const& factory_config) {
    out = from_yaml<derive_base_type_t<PT>>(factory_config);
}

}  // namespace yadi

#endif  // YADI_CREATE_UTILS_HPP
