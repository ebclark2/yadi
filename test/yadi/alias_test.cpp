//
// Created by Ed Clark on 8/18/17.
//

#include <iostream>
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

YADI_TEST(aliases_config_test) {
    yadi::register_type<YAML::Node, YAML::Node>("yaml_base");
    std::string const aliases_yaml = R"raw(
yaml1:
  type: "yaml_base"
  config: "yaml1_config"
yaml2:
  type: "yaml_base"
  config: "yaml2_config"
)raw";
    register_aliases<YAML::Node>(YAML::Load(aliases_yaml));
    {
        auto ymlPtr = factory<YAML::Node>::create("yaml1");
        if (!ymlPtr) {
            return false;
        }
        if (!(ymlPtr->as<std::string>("Not a string") == "yaml1_config")) {
            return false;
        }
    }

    {
        auto ymlPtr = factory<YAML::Node>::create("yaml2");
        if (!ymlPtr) {
            return false;
        }
        if (!(ymlPtr->as<std::string>("Not a string") == "yaml2_config")) {
            return false;
        }
    }

    return true;
}
}  // namespace yadi