//
// Created by ebclark on 6/29/18.
//

#include "test.hpp"

namespace yadi {
namespace {

struct Bar {
    explicit Bar(int i) : i(i) { }

    int i;
};

struct Foo {
    explicit Foo(Bar bar) : bar(bar) {}

    Bar bar;
};



YADI_TEST(passthrough) {
    Foo foo = ::yadi::from_yaml<Foo>(YAML::Load("5"));
    YADI_ASSERT_EQ(5, foo.bar.i);

    return true;
}

} // anonymous namespace

template <>
struct factory_traits<Foo> {
    using ptr_type = Foo;
    static constexpr bool direct_from_yaml = true;
};

YADI_INIT_BEGIN
::yadi::register_type<Foo>(::yadi::type_by_value_key(), ::yadi::make_single_arg_initializer<Foo>(&::yadi::ctr<Foo, Foo, ::yadi::passthrough<Bar, int>>));
YADI_INIT_END

} // namespace yadi