//
// Created by Ed Clark on 8/29/17.
//

#ifndef YADI_INITIALIZERS_HPP
#define YADI_INITIALIZERS_HPP

#include "factory.hpp"
namespace yadi {

/**
 * @brief Returns config.as<T>().  Signature matches factory initializer.
 * @tparam T
 * @param config
 * @return
 */
template <typename T>
T yaml_as(YAML::Node const& config);

// TODO comment
template <typename T>
yadi_info_t<T> yaml_as_with_help();

/**
 * @brief Constructs IT via a constructor that accepts YAML and returns as pointer to BT,
 * ptr_type_t<BT>.
 * @tparam BT Use to determine pointer type.
 * @tparam IT Implementation type.  Type to construct
 * @param config Argument to constructor.
 * @return The constructed type as ptr_type_t<BT>
 */
template <typename BT, typename IT>
ptr_type_t<BT> yaml_init(YAML::Node const& config);

template <typename BT, typename IT>
ptr_type_t<BT> no_arg_init(YAML::Node const&);

// ################### IMPL ######################

template <typename T>
T yaml_as(YAML::Node const& config) {
    // TODO Improved error message
    return config.as<T>();
}

template <typename T>
yadi_info_t<T> yaml_as_with_help() {
    // TODO Improved error message
    return {&yaml_as<T>, "Direct conversion using yaml.as<T>()"};
}

template <typename BT, typename IT,
          typename = typename std::enable_if<is_by_value<BT>::value && std::is_same<BT, IT>::value>::type>
struct yaml_init_value {
    static ptr_type_t<BT> init(YAML::Node const& config) {
        BT ret(config);
        return ret;
    }
};

template <typename BT, typename IT, bool by_value = is_by_value<BT>::value>
struct yaml_init_helper {};

template <typename BT, typename IT>
struct yaml_init_helper<BT, IT, false> {
    static ptr_type_t<BT> init(YAML::Node const& config) {
        ptr_type_t<BT> ret(new IT(config));
        return ret;
    }
};

template <typename BT, typename IT>
struct yaml_init_helper<BT, IT, true> {
    static ptr_type_t<BT> init(YAML::Node const& config) { return yaml_init_value<BT, IT>::init(config); }
};

template <typename BT, typename IT>
ptr_type_t<BT> yaml_init(YAML::Node const& config) {
    return yaml_init_helper<BT, IT>::init(config);
};

template <typename BT, typename IT,
          typename = typename std::enable_if<is_by_value<BT>::value && std::is_same<BT, IT>::value>::type>
struct no_arg_init_value {
    static ptr_type_t<BT> init() {
        BT ret;
        return ret;
    }
};

template <typename BT, typename IT, bool by_value = is_by_value<BT>::value>
struct no_arg_init_helper {};

template <typename BT, typename IT>
struct no_arg_init_helper<BT, IT, false> {
    static ptr_type_t<BT> init() {
        ptr_type_t<BT> ret(new IT);
        return ret;
    }
};

template <typename BT, typename IT>
struct no_arg_init_helper<BT, IT, true> {
    static ptr_type_t<BT> init() { return no_arg_init_value<BT, IT>::init(); }
};

template <typename BT, typename IT>
ptr_type_t<BT> no_arg_init(YAML::Node const&) {
    return no_arg_init_helper<BT, IT>::init();
};

}  // namespace yadi

#endif  // YADI_INITIALIZERS_HPP
