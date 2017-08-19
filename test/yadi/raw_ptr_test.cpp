//
// Created by Ed Clark on 8/17/17.
//

#include <yadi/factory.hpp>
#include "test.hpp"

namespace yadi {

struct my_raw_ptr;

template <>
struct factory_traits<my_raw_ptr> {
  using ptr_type = my_raw_ptr *;
};

struct my_raw_ptr : public factory<my_raw_ptr> {};

YADI_TEST(raw_ptr_create) {
  register_type_no_arg<my_raw_ptr, my_raw_ptr>("raw");
  my_raw_ptr::ptr_type p = my_raw_ptr::create("raw");
  return p;
}

}  // namespace yadi
