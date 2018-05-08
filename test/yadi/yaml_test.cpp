//
// Created by Ed Clark on 8/18/17.
//

#include "test.hpp"

#include <vector>

namespace yadi {

YADI_TEST(yaml_argument_test) {
    YAML::Node expected = YAML::Load("Hello World!");
    YAML::Node yml = factory<YAML::Node>::create("yaml", expected);
    if (!yml) {
        return false;
    }
    return yml == expected;
}

}  // namespace yadi
