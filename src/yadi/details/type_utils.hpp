//
// Created by Ed Clark on 8/29/17.
//

#ifndef YADI_TYPE_UTILS_HPP
#define YADI_TYPE_UTILS_HPP

#include "factory.hpp"

#include <type_traits>

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
 * @brief Determine is factory returns by value
 * @tparam BT
 */
template <typename BT>
struct is_by_value {
    static const bool value = std::is_same<BT, ptr_type_t<BT>>::value;
};

/**
 * @brief Helper to derive_base_type specialization by value.  Adds check to verify factory is actually by value.
 * @tparam T
 */
template <typename T, typename E = std::enable_if_t<is_by_value<T>::value>>
struct derive_base_type_by_value;

template <typename T>
struct derive_base_type_by_value<T, std::enable_if_t<is_by_value<T>::value>> {
    using base_type = T;
};

/**
 * For type T, provide the factory base type that creates T. For example, for T std::unique_ptr<int>, int would
 * be the base type.
 * @tparam T
 */
template <typename T>
struct derive_base_type {
    using base_type = bare_t<T>;
};

template <typename T>
struct derive_base_type<std::shared_ptr<T>> {
    using base_type = is_same_then_t<ptr_type_t<T>, std::shared_ptr<T>, bare_t<T>>;
};

template <typename T>
struct derive_base_type<std::unique_ptr<T>> {
    using base_type = is_same_then_t<ptr_type_t<T>, std::unique_ptr<T>, bare_t<T>>;
};

template <typename T>
struct derive_base_type<T*> {
    using base_type = is_same_then_t<ptr_type_t<T>, T*, bare_t<T>>;
};

template <typename T>
using derive_base_type_t = typename derive_base_type<T>::base_type;

// SHENANIGANS
template <typename OT, bool IBT = !is_by_value<derive_base_type_t<OT>>::value &&
                                  std::is_same<bare_t<OT>, derive_base_type_t<OT>>::value>
struct derive_output_type;

template <typename OT>
struct derive_output_type<OT, true> {
    using type = ptr_type_t<OT>;
};

template <typename OT>
struct derive_output_type<OT, false> {
    using type = OT;
};

template <typename OT>
using derive_output_type_t = typename derive_output_type<OT>::type;

}  // namespace yadi
#endif  // YADI_TYPE_UTILS_HPP
