//
// Created by Ed Clark on 8/18/17.
//

#include "test.hpp"

#include <vector>

namespace yadi {
YADI_INIT_BEGIN
register_type<YAML::Node, YAML::Node>("yaml");
YADI_INIT_END

YADI_TEST(yaml_argument_test) {
    YAML::Node expected = YAML::Load("Hello World!");
    ptr_type_t<YAML::Node> ymlPtr = factory<YAML::Node>::create("yaml", expected);
    if (!ymlPtr) {
        return false;
    }
    return *ymlPtr == expected;
}

YADI_TEST(from_yaml_string) {
    auto ret = from_yaml<YAML::Node>(YAML::Load("yaml"));
    return ret.get();
}

YADI_TEST(from_yaml_map) {
    std::string yaml_factory_config = R"raw(
type: "yaml"
config: "Hello World!"
)raw";
    ptr_type_t<YAML::Node> ymlPtr = from_yaml<YAML::Node>(YAML::Load(yaml_factory_config));
    if (!ymlPtr) {
        return false;
    }
    return ymlPtr->as<std::string>("Not a string") == "Hello World!";
}

YADI_TEST(from_yaml_sequence) {
    std::string yaml_factory_config = R"raw(
---
- type: "yaml"
  config: "type1"
- type: "yaml"
  config: "type2"
)raw";

    std::vector<ptr_type_t<YAML::Node>> yamls;
    from_yamls<YAML::Node>(YAML::Load(yaml_factory_config), std::back_inserter(yamls));
    YADI_ASSERT_EQ(2u, yamls.size());
    YADI_ASSERT_EQ("type1", yamls.at(0)->as<std::string>());
    YADI_ASSERT_EQ("type2", yamls.at(1)->as<std::string>());

    return true;
}

}  // namespace yadi