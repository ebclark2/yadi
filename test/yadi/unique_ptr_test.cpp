//
// Created by Ed Clark on 8/17/17.
//

#include <yadi/factory.hpp>
#include "test.hpp"

namespace yadi {

    struct my_unique_ptr;

    template <> struct factory_traits<my_unique_ptr> {
        using ptr_type = std::unique_ptr<my_unique_ptr>;
    };

    struct my_unique_ptr : public factory<my_unique_ptr> {};

    YADI_TEST(unique_ptr_create)   {
            my_unique_ptr::register_type<my_unique_ptr>("unique");
            my_unique_ptr::ptr_type p = my_unique_ptr::create("unique");
            return p.get();
    }


} // namespace yadi

