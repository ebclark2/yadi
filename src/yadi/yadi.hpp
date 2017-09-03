#ifndef YADI_FACTORY_HPP__
#define YADI_FACTORY_HPP__

#include "details/create_specializations.hpp"
#include "details/create_utils.hpp"
#include "details/demangle.hpp"
#include "details/factory.hpp"
#include "details/help.hpp"
#include "details/initializers.hpp"
#include "details/registration.hpp"

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
