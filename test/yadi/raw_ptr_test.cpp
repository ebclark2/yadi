//
// Created by Ed Clark on 8/17/17.
//

#include "test.hpp"

namespace yadi {

struct my_raw_type;

template <>
struct factory_traits<my_raw_type> {
    using ptr_type = my_raw_type*;
};

struct my_raw_type : public factory<my_raw_type> {};

YADI_TEST(raw_ptr_create) {
    register_type_no_arg<my_raw_type, my_raw_type>("raw");
    my_raw_type* p = my_raw_type::create("raw");
    return p;
}

}  // namespace yadi
