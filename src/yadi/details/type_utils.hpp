//
// Created by Ed Clark on 8/29/17.
//

#ifndef YADI_TYPE_UTILS_HPP
#define YADI_TYPE_UTILS_HPP

#include "factory.hpp"

#include <type_traits>

/**
 * @namespace yadi
 * @brief YADI
 */
namespace yadi {
namespace meta {
template <typename T>
using bare_t = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

/**
 * If lest is same as right then provide type_type as type.
 * @tparam L left
 * @tparam R right
 * @tparam RT result type
 */
template <typename L, typename R, typename RT,
          typename = typename std::enable_if<std::is_convertible<L, R>::value>::type>
struct if_convertible_then {
    using type = RT;
};

template <typename L, typename R, typename RT>
using if_convertible_then_t = typename if_convertible_then<L, R, RT>::type;

/**
 * @brief Determine is factory returns by value
 * @tparam BT
 */
template <typename BT>
struct is_by_value {
    static const bool value = std::is_same<BT, ptr_type_t<BT>>::value;
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
using derive_base_type_t = typename derive_base_type<T>::base_type;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
template <typename T>
struct derive_base_type<std::shared_ptr<T>> {
    using base_type = bare_t<T>;  // if_convertible_then_t<ptr_type_t<T>, std::shared_ptr<T>, bare_t<T>>;
};

template <typename T>
struct derive_base_type<std::unique_ptr<T>> {
    using base_type = bare_t<T>;  // if_convertible_then_t<ptr_type_t<T>, std::unique_ptr<T>, bare_t<T>>;
};

template <typename T>
struct derive_base_type<T*> {
    using base_type = bare_t<T>;  // if_convertible_then_t<ptr_type_t<T>, T*, bare_t<T>>;
};
#endif  // DOXYGEN_SHOULD_SKIP_THIS

}  // namespace meta
}  // namespace yadi
#endif  // YADI_TYPE_UTILS_HPP
