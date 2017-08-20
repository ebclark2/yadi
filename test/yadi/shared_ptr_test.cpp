//
// Created by Ed Clark on 8/17/17.
//

#include "test.hpp"

namespace yadi {

struct my_shared_type {};

template <>
struct factory_traits<my_shared_type> {
    using ptr_type = std::unique_ptr<my_shared_type>;
};

struct my_shared_type_1 : public my_shared_type {};

YADI_TEST(shared_ptr_create) {
    register_type_no_arg<my_shared_type, my_shared_type_1>("my_type_1");
    std::shared_ptr<my_shared_type> p = factory<my_shared_type>::create("my_type_1");
    return p.get();
}

}  // namespace yadi
