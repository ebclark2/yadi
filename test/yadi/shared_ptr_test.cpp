//
// Created by Ed Clark on 8/17/17.
//

#include "test.hpp"

namespace yadi {

struct my_shared_type {};

template <>
struct factory_traits<my_shared_type> {
    using ptr_type = std::shared_ptr<my_shared_type>;
    static const bool direct_from_yaml = false;
};

struct my_shared_type_1 : public my_shared_type {};

YADI_INIT_BEGIN
register_type_no_arg<my_shared_type, my_shared_type_1>("my_type_1");
register_type<my_shared_type>("cached_type",
                              make_caching_initializer<my_shared_type>(&no_arg_init<my_shared_type, my_shared_type_1>));
register_type<my_shared_type>("cached_type_with_help",
                              make_caching_initializer<my_shared_type>({&no_arg_init<my_shared_type, my_shared_type_1>,
                                                                        "No config"}));
YADI_INIT_END

YADI_TEST(shared_ptr_create) {
    std::shared_ptr<my_shared_type> p = factory<my_shared_type>::create("my_type_1");
    return p.get();
}

YADI_TEST(shared_ptr_parse) {
    std::shared_ptr<my_shared_type> p;
    parse(p, YAML::Load("my_type_1"));
    return p.get();
}

YADI_TEST(cache_test) {
    std::shared_ptr<my_shared_type> p1 = ::yadi::create<my_shared_type>("cached_type");
    std::shared_ptr<my_shared_type> p2 = ::yadi::create<my_shared_type>("cached_type");
    YADI_ASSERT_EQ(p1, p2);

    return true;
}

YADI_TEST(cache_with_help_test) {
    std::shared_ptr<my_shared_type> p1 = ::yadi::create<my_shared_type>("cached_type_with_help");
    std::shared_ptr<my_shared_type> p2 = ::yadi::create<my_shared_type>("cached_type_with_help");
    YADI_ASSERT_EQ(p1, p2);

    return true;
}
}  // namespace yadi
