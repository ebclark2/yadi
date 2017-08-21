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

}  // namespace yadi
