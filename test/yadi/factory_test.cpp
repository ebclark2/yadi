//
// Created by Ed Clark on 8/17/17.
//

#include <yadi/factory.hpp>

namespace yadi {

    struct my_type {
    };

    struct my_type_1 : public my_type {
    };

    struct my_raw_ptr;

    template <>
    struct factory_traits<my_raw_ptr>
    {
        using ptr_type = std::unique_ptr<my_raw_ptr>;
    };

    struct my_raw_ptr : public factory<my_raw_ptr> { };

    int main() {
        {
            factory<my_type>::register_type<my_type_1>("my_type_1");
            ptr_type_t<my_type> p = factory<my_type>::create("my_type_1");
        }

        {
            my_raw_ptr::register_type<my_raw_ptr>("raw");
            my_raw_ptr::ptr_type p = my_raw_ptr::create("raw");
        }

        return 0;
    }

} // namespace yadi