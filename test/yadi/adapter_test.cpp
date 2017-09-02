//
// Created by Ed Clark on 8/31/17.
//

#include "test.hpp"

#include <list>
#include <type_traits>

namespace yadi {

YADI_TEST(adapter_test) {
    int five = adapter<int>::create(type_by_value_key(), YAML::Load("5"));
    YADI_ASSERT_EQ(5, five);

    std::list<int> fives = adapter<std::list<int>, std::list<int>>::create("", YAML::Load("[5, 5, 5, 5, 5]"));
    YADI_ASSERT_EQ(5, fives.size());

    return true;
}

}  // namespace yadi