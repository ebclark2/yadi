#ifndef YADI_FACTORY_HPP__
#define YADI_FACTORY_HPP__

#include "details/create_specializations.hpp"
#include "details/create_utils.hpp"
#include "details/demangle.hpp"
#include "details/factory.hpp"
#include "details/help.hpp"
#include "details/initializers.hpp"
#include "details/registration.hpp"

#include <cstdint>

/**
 * @namespace yadi
 * @brief YADI
 */
namespace yadi {}
YADI_YAML_TYPE_BY_VALUE_DECL(YAML::Node, YAML_Node)

#ifndef YADI_NO_STD_STRING
YADI_YAML_TYPE_BY_VALUE_DECL(std::string, string)
#endif  // YADI_NO_STD_STRING
#ifndef YADI_NO_INT
YADI_YAML_TYPE_BY_VALUE_DECL(int8_t, int8_t)
YADI_YAML_TYPE_BY_VALUE_DECL(int16_t, int16_t)
YADI_YAML_TYPE_BY_VALUE_DECL(int32_t, int32_t)
YADI_YAML_TYPE_BY_VALUE_DECL(int64_t, int64_t)
YADI_YAML_TYPE_BY_VALUE_DECL(uint8_t, uint8_t)
YADI_YAML_TYPE_BY_VALUE_DECL(uint16_t, uint16_t)
YADI_YAML_TYPE_BY_VALUE_DECL(uint32_t, uint32_t)
YADI_YAML_TYPE_BY_VALUE_DECL(uint64_t, uint64_t)
#endif  // YADI_NO_INT
#ifndef YADI_NO_FLOAT
YADI_YAML_TYPE_BY_VALUE_DECL(float, float)
YADI_YAML_TYPE_BY_VALUE_DECL(double, double)
#endif  // YADI_NO_FLOAT
#ifndef YADI_NO_BOOL
YADI_YAML_TYPE_BY_VALUE_DECL(bool, bool)
#endif  // YADI_NO_BOOL

#endif  // YADI_FACTORY_HPP__
