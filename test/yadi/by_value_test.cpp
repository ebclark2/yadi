//
// Created by Ed Clark on 8/20/17.
//

#include "test.hpp"

#include <string>

namespace yadi {

template <>
struct factory_traits<std::string> {
    using ptr_type = std::string;
};

YADI_TEST(by_value) {
    ::yadi::register_type<std::string>("std::string", &yaml_as<std::string>);
    std::string str = factory<std::string>::create("std::string", YAML::Load("Hello World!"));
    YADI_ASSERT_EQ("Hello World!", str);
    return true;
}

}  // namespace yadi