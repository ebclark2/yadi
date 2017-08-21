//
// Created by Ed Clark on 8/20/17.
//

#include <yadi/std_types.hpp>
#include "test.hpp"

#include <string>

namespace yadi {

YADI_TEST(by_value) {
    std::string str = from_yaml<std::string>(YAML::Load("Hello World!"));
    YADI_ASSERT_EQ("Hello World!", str);

    int i = from_yaml<int>(YAML::Load("56"));
    YADI_ASSERT_EQ(56, i);

    std::string out;
    parse(out, YAML::Load("asdf"));
    YADI_ASSERT_EQ("asdf", out);
    return true;
}

}  // namespace yadi