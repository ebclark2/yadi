//
// Created by Ed Clark on 8/29/17.
//

#ifndef YADI_INITIALIZERS_HPP
#define YADI_INITIALIZERS_HPP

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

}  // namespace yadi

#endif  // YADI_INITIALIZERS_HPP
