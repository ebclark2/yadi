//
// Created by Ed Clark on 8/28/17.
//

#include "yadi.hpp"

#include <string>

#ifndef YADI_NO_STD_STRING
YADI_YAML_TYPE_BY_VALUE_DEFN(std::string, string)
#endif
#ifndef YADI_NO_STD_INT
YADI_YAML_TYPE_BY_VALUE_DEFN(int, int)
#endif
#ifndef YADI_NO_STD_FLOAT
YADI_YAML_TYPE_BY_VALUE_DEFN(float, float)
YADI_YAML_TYPE_BY_VALUE_DEFN(double, double)
#endif
