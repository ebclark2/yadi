//
// Created by Ed Clark on 8/21/17.
//

#include "test.hpp"

double sum(double a1, double a2, int a3, double a4) { return a1 + a2 + a3 + a4; }

struct Foo {
    static double sum(double const a1, double const& a2, int&& a3, double a4) { return a1 + a2 + a3 + a4; }
};

YADI_TEST(yaml_bindings_test) {
    std::string YAML = R"raw(
---
- 1.5
- 2.5
- 3
- 4.5
)raw";
    using ctr_sig = std::function<double(double, double, int, double)>;

    ::yadi::details::function_traits_params_type<ctr_sig> test;
    ::yadi::details::yaml_to_tuple<decltype(test)>::to_tuple(test, YAML::Load(YAML));
    YADI_ASSERT_EQ(std::get<0>(test), 1.5);
    YADI_ASSERT_EQ(std::get<1>(test), 2.5);
    YADI_ASSERT_EQ(std::get<2>(test), 3);
    YADI_ASSERT_EQ(std::get<3>(test), 4.5);

    {
        double result = ::yadi::details::call_from_yaml(&sum, YAML::Load(YAML));
        YADI_ASSERT_EQ(11.5, result);
    }

    {
        double result = ::yadi::details::call_from_yaml(&Foo::sum, YAML::Load(YAML));
        YADI_ASSERT_EQ(11.5, result);
    }

    {
        // create initializer type the factory will use...
        ::yadi::register_type<double>("sum_of_4", ::yadi::make_sequence_initializer<double>(&sum));
        YADI_ASSERT_EQ(11.5, ::yadi::create<double>("sum_of_4", YAML::Load(YAML)));
    }

    return true;
}

YADI_TEST(yaml_bindings_map_test) {
    std::string YAML = R"raw(
---
a1: 1.5
a3: 3
a2: 2.5
a4: 4.5
)raw";
    ::yadi::register_type<double>("mapped_sum", ::yadi::make_map_initializer<double>(&sum, {"a1", "a2", "a3", "a4"}));
    YADI_ASSERT_EQ(11.5, ::yadi::create<double>("mapped_sum", YAML::Load(YAML)));

    return true;
}
