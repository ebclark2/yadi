//
// Created by Ed Clark on 8/20/17.
//

#ifndef YADI_STD_TYPES_HPP_HPP
#define YADI_STD_TYPES_HPP_HPP

#include "yadi.hpp"

namespace yadi {

#define TYPE_BY_VALUE(TYPE, INIT_NAME)               \
    template <>                                      \
    struct factory_traits<TYPE> {                    \
        using ptr_type = TYPE;                       \
        static const bool direct_from_yaml = true;   \
    };                                               \
                                                     \
    YADI_INIT_BEGIN_N(INIT_NAME)                     \
    ::yadi::register_type<TYPE>("", &yaml_as<TYPE>); \
    YADI_INIT_END_N(INIT_NAME)

TYPE_BY_VALUE(std::string, string)
TYPE_BY_VALUE(int, int)

}  // namespace yadi

#endif  // YADI_STD_TYPES_HPP_HPP
