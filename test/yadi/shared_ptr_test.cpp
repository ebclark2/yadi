//
// Created by Ed Clark on 8/17/17.
//

#include <yadi/factory.hpp>
#include "test.hpp"

namespace yadi {

struct my_type {};

struct my_type_1 : public my_type {};

YADI_TEST(shared_ptr_create) {
  factory<my_type>::register_type<my_type_1>("my_type_1");
  ptr_type_t<my_type> p = factory<my_type>::create("my_type_1");
  return p.get();
}

}  // namespace yadi
