#ifndef YADI_FACTORY_HPP__
#define YADI_FACTORY_HPP__

#include "details/create_utils.hpp"
#include "details/demangle.hpp"
#include "details/factory.hpp"
#include "details/factory_specializations.hpp"
#include "details/help.hpp"
#include "details/initializers.hpp"
#include "details/registration.hpp"

#include <yaml-cpp/yaml.h>

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#define YADI_INIT_BEGIN_N(NAME)           \
    namespace {                           \
    struct static_initialization_##NAME { \
        static_initialization_##NAME() {
#define YADI_INIT_END_N(NAME)                                      \
    }                                                              \
    }                                                              \
    ;                                                              \
    static_initialization_##NAME static_initialization_##NAME##__; \
    }

#define YADI_INIT_BEGIN YADI_INIT_BEGIN_N(ANON)
#define YADI_INIT_END YADI_INIT_END_N(ANON)

/// Expose types yaml supports directly
#define YADI_YAML_TYPE_BY_VALUE_DECL(TYPE, INIT_NAME) \
    template <>                                       \
    struct ::yadi::factory_traits<TYPE> {             \
        using ptr_type = TYPE;                        \
        static const bool direct_from_yaml = true;    \
    };

#define YADI_YAML_TYPE_BY_VALUE_DEFN(TYPE, INIT_NAME)                                            \
    YADI_INIT_BEGIN_N(INIT_NAME)                                                                 \
    ::yadi::register_type<TYPE>(::yadi::type_by_value_key(), ::yadi::yaml_as_with_help<TYPE>()); \
    ::yadi::register_factory<TYPE>(#TYPE);                                                       \
    YADI_INIT_END_N(INIT_NAME)

#ifndef YADI_NO_STD_STRING
YADI_YAML_TYPE_BY_VALUE_DECL(std::string, string)
#endif  // YADI_NO_STD_STRING
#ifndef YADI_NO_STD_STRING
YADI_YAML_TYPE_BY_VALUE_DECL(int, int)
#endif  // YADI_NO_STD_STRING
#ifndef YADI_NO_STD_DOUBLE
YADI_YAML_TYPE_BY_VALUE_DECL(double, double)
#endif  // YADI_NO_STD_DOUBLE

#endif  // YADI_FACTORY_HPP__
