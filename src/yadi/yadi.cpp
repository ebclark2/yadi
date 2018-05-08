//
// Created by Ed Clark on 8/28/17.
//

#include "yadi.hpp"

#include <string>

YADI_YAML_TYPE_BY_VALUE_DEFN(YAML::Node, YAML_Node)

#ifndef YADI_NO_STD_STRING
YADI_YAML_TYPE_BY_VALUE_DEFN(std::string, string)
#endif
#ifndef YADI_NO_INT
YADI_YAML_TYPE_BY_VALUE_DEFN(int8_t, int8_t)
YADI_YAML_TYPE_BY_VALUE_DEFN(int16_t, int16_t)
YADI_YAML_TYPE_BY_VALUE_DEFN(int32_t, int32_t)
YADI_YAML_TYPE_BY_VALUE_DEFN(int64_t, int64_t)
YADI_YAML_TYPE_BY_VALUE_DEFN(uint8_t, uint8_t)
YADI_YAML_TYPE_BY_VALUE_DEFN(uint16_t, uint16_t)
YADI_YAML_TYPE_BY_VALUE_DEFN(uint32_t, uint32_t)
YADI_YAML_TYPE_BY_VALUE_DEFN(uint64_t, uint64_t)
#endif
#ifndef YADI_NO_FLOAT
YADI_YAML_TYPE_BY_VALUE_DEFN(float, float)
YADI_YAML_TYPE_BY_VALUE_DEFN(double, double)
#endif
#ifndef YADI_NO_BOOL
YADI_YAML_TYPE_BY_VALUE_DEFN(bool, bool)
#endif
