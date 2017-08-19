//
// Created by Ed Clark on 8/18/17.
//

#include "test.hpp"

namespace yadi {

YADI_TEST(alias_config_test) {
    yadi::register_type<YAML::Node, YAML::Node>("yaml_type");
    register_alias<YAML::Node>("yaml_alias", "yaml_type", YAML::Load("Hello World!"));
    auto ymlPtr = factory<YAML::Node>::create("yaml_alias");
    if (!ymlPtr) {
        return false;
    }
    return ymlPtr->as<std::string>("Not a string") == "Hello World!";
}
}  // namespace yadi