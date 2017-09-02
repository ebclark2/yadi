//
// Created by Ed Clark on 8/17/17.
//

#include "test.hpp"

namespace yadi {

struct my_unique_type;

struct my_unique_type : public factory<my_unique_type> {};

YADI_INIT_BEGIN
register_type_no_arg<my_unique_type, my_unique_type>("unique");
YADI_INIT_END

YADI_TEST(unique_ptr_create) {
    std::unique_ptr<my_unique_type> p = my_unique_type::create("unique");
    std::shared_ptr<my_unique_type> shared_p = ::yadi::create<std::shared_ptr<my_unique_type>>("unique");
    return p.get() && shared_p.get();
}

YADI_TEST(unique_ptr_parse) {
    std::unique_ptr<my_unique_type> p;
    parse(p, YAML::Load("unique"));
    return p.get();
}

}  // namespace yadi
