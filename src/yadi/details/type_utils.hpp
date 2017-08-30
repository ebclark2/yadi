//
// Created by Ed Clark on 8/29/17.
//

#ifndef YADI_TYPE_UTILS_HPP
#define YADI_TYPE_UTILS_HPP

namespace yadi {

template <typename T>
using bare_t = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

/**
 * If lest is same as right then provide type_type as type.
 * @tparam L left
 * @tparam R right
 * @tparam RT result type
 */
template <typename L, typename R, typename RT, typename = typename std::enable_if<std::is_same<L, R>::value>::type>
struct if_same_then {
    using type = RT;
};

template <typename L, typename R, typename RT>
using is_same_then_t = typename if_same_then<L, R, RT>::type;

/**
 * @brief Helper to derive_base_type specialization by value.  Adds check to verify factory is actually by value.
 * @tparam T
 */
template <typename T, typename = typename std::enable_if<is_by_value<T>::value>::type>
struct derive_base_type_by_value {
    using base_type = T;
};

/**
 * For type T, provide the factory base type that creates T. For example, for T std::unique_ptr<int>, int would
 * be the base type.
 * @tparam T
 */
template <typename T>
struct derive_base_type {
    using base_type = typename derive_base_type_by_value<T>::base_type;
};

template <typename T>
using derive_base_type_t = typename derive_base_type<T>::base_type;

}  // namespace yadi
#endif  // YADI_TYPE_UTILS_HPP
