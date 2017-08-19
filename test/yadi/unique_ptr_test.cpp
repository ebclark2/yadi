//
// Created by Ed Clark on 8/17/17.
//

#include "test.hpp"

namespace yadi {

struct my_unique_type;

template <>
struct factory_traits<my_unique_type> {
    using ptr_type = std::unique_ptr<my_unique_type>;
};

struct my_unique_type : public factory<my_unique_type> {};

YADI_TEST(unique_ptr_create) {
    register_type_no_arg<my_unique_type, my_unique_type>("unique");
    std::unique_ptr<my_unique_type> p = my_unique_type::create("unique");
    return p.get();
}

}  // namespace yadi
