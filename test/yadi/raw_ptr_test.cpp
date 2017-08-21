//
// Created by Ed Clark on 8/17/17.
//

#include "test.hpp"

namespace yadi {

struct my_raw_type;

template <>
struct factory_traits<my_raw_type> {
    using ptr_type = my_raw_type*;
    static const bool direct_from_yaml = false;
};

struct my_raw_type : public factory<my_raw_type> {};

YADI_INIT_BEGIN
register_type_no_arg<my_raw_type, my_raw_type>("raw");
YADI_INIT_END

YADI_TEST(raw_ptr_create) {
    my_raw_type* p = my_raw_type::create("raw");
    YADI_ASSERT_NE(0, p);
    delete p;
    return true;
}

YADI_TEST(raw_ptr_parse) {
    my_raw_type* p;
    parse(p, YAML::Load("raw"));
    YADI_ASSERT_NE(0, p);
    delete p;
    return true;
}

}  // namespace yadi
