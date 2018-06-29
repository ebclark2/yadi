//
// Created by ebclark on 5/23/18.
//

#include "test.hpp"

namespace yadi {

struct RMFoo {
    int foo;
};

template <>
struct factory_traits<RMFoo> {
    using ptr_type = RMFoo;
    static constexpr bool direct_from_yaml = true;
};

// Change int initialization to always return 7, because 7 is the best number
template <>
struct registration_mod<RMFoo> {
    static yadi_info_t<RMFoo> mod(yadi_info_t<RMFoo> yadis) {
        yadis.initializer = [](auto const&) { return RMFoo{7}; };
        return yadis;
    }
};

YADI_INIT_BEGIN

// Register to always create 10
::yadi::register_type<RMFoo>(::yadi::type_by_value_key(), [](auto const&) { return RMFoo{10};});

YADI_INIT_END

namespace {

YADI_TEST(registration_mod_test) {
    RMFoo i = ::yadi::from_yaml<RMFoo>(YAML::Node{});
    YADI_ASSERT_EQ(7, i.foo);

    return true;
}

} // anonymous namespace
} // namespace yadi