//
// Created by Ed Clark on 8/29/17.
//

#include "test.hpp"

YADI_TEST(parse_test) {
    int out;
    ::yadi::parse(out, YAML::Load("5"));
    YADI_ASSERT_EQ(5, out);

    return true;
}