//
// Created by Ed Clark on 8/18/17.
//

#include "test.hpp"

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

YADI_TEST(yaml_factory_config_string) {
  auto ret = factory<YAML::Node>::create(YAML::Load("yaml"));
  return ret.get();
}

YADI_TEST(yaml_factory_config_map) {
  char const* yaml_factory_config = R"raw(
type: "yaml"
config: "Hello World!"
)raw";
  ptr_type_t<YAML::Node> ymlPtr =
      factory<YAML::Node>::create(YAML::Load(yaml_factory_config));
  if (!ymlPtr) {
    return false;
  }
  return ymlPtr->as<std::string>("Not a string") == "Hello World!";
}

}  // namespace yadi