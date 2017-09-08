//
// Created by Ed Clark on 8/29/17.
//

#ifndef YADI_CREATE_UTILS_HPP
#define YADI_CREATE_UTILS_HPP

#include "factory.hpp"
#include "help.hpp"
#include "type_utils.hpp"

#include <yaml-cpp/yaml.h>

/**
 * @namespace yadi
 * @brief YADI
 */
namespace yadi {

/**
 * @brief Returns the key used for initializers creating by value directly from yaml.
 * @return The key.
 */
std::string const& type_by_value_key();

/**
 * @brief Provides a layer above factory<BT>::create.  This allows types such as templated containers to
 * be created without registering initializers for each element type.
 * @tparam FT The type the base type will be derived from via derived_base_type_t<FT>.
 * @tparam OT The desired output type.
 */
template <typename FT, typename OT = ptr_type_t<meta::derive_base_type_t<FT>>>
struct adapter {
    using base_type = meta::derive_base_type_t<FT>;
    using output_type = OT;

    // TODO add some function to show factory return type in error message
    static_assert(std::is_convertible<ptr_type_t<base_type>, OT>::value,
                  "Unable to convert factory return type to desired output type in ");

    /**
     * @brief The default is to forward to factory<base_type>::create(type, config);
     * @param type
     * @param config
     * @return
     */
    static output_type create(std::string const& type, YAML::Node const& config = {}) {
        return factory<base_type>::create(type, config);
    }

    static std::string get_name() { return yadi_help::get_name<base_type>(); }
};

/**
 * @brief Same as adapter<FT>::create(type, config);
 * @tparam FT Type used to derive factory
 * @param type
 * @param config
 * @return
 */
template <typename FT>
typename adapter<FT>::output_type create(std::string const& type, YAML::Node const& config = {});

/**
 * @brief Pulls type and config from YAML.  This function is especially usefil when loading
 * nested types from YAML configuration.  If factory_config is a scalar string it will be used
 * as type.  If factory_config is a map then "type" and "config" keys will be pulled from it and
 * used as such, unless the base type indicates it should be created directly from yaml.  In this case
 * factory_config is used as the config passed to the adapter.
 * @tparam OT The desired output type.  The factory base type will be derived from this using
 * derive_base_type_t<OT>.
 * @param factory_config
 * @return
 */
template <typename OT>
OT from_yaml(YAML::Node const& factory_config);

/**
 * @brief Equivalent to from_yaml<ptr_type_t<base_type>>(config)
 * @tparam BT The factory baes type
 * @param config
 * @return
 */
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

/**
 * @brief Equivalent to from_yamls<ptr_type_t<base_type>>(factory_configs, out);
 * @tparam BT base type
 * @tparam OI output iterator
 * @param factory_configs
 * @param out
 */
template <typename BT, typename OI>
void from_yamls_base(YAML::Node const& factory_configs, OI out) {
    from_yamls<ptr_type_t<BT>>(factory_configs, out);
}

/**
 * @brief Populate out from factory config.  The factory type is derived from ptr_type.
 * @tparam OT output type
 * @param out
 * @param factory_config
 */
template <typename OT>
void parse(OT& out, YAML::Node const& factory_config);

// ################# IMPL #####################
template <typename FT>
typename adapter<FT>::output_type create(std::string const& type, YAML::Node const& config) {
    return adapter<FT>::create(type, config);
}

template <typename OT>
OT from_yaml(YAML::Node const& factory_config) {
    using BT = meta::derive_base_type_t<OT>;
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

}  // namespace yadi

#endif  // YADI_CREATE_UTILS_HPP
