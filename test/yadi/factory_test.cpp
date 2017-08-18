//
// Created by Ed Clark on 8/17/17.
//

#include <yadi/factory.hpp>

struct my_type { };

struct my_type_1 : public my_type {
};

int main()
{
    yadi::factory<my_type>::register_type<my_type_1>("my_type_1");
    yadi::ptr_type_t<my_type> p = yadi::factory<my_type>::create("my_type_1");

    return 0;
}