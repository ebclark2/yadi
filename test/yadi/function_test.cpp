//
// Created by Ed Clark on 8/21/17.
//

#include "test.hpp"

#include <functional>
#include <iostream>

/*
 * Experiments that hopefully lead to building a tuple from a YAML sequence based on the param pack
 * of a std function. This could then be unpacked to call some function giving a binding
 * implementation of sorts, so long as I don't forget what I'm doing.
 */

template <typename tuple_t, size_t index = std::tuple_size<tuple_t>::value - 1>
struct print_thing {
    static void print(YAML::Node const& yaml) {
        using element_type = std::tuple_element_t<std::tuple_size<tuple_t>::value - 1 - index, tuple_t>;
        std::cout << ::yadi::from_yaml<element_type>(yaml[std::tuple_size<tuple_t>::value - 1 - index]) << '\n';
        print_thing<tuple_t, index - 1>::print(yaml);
    }

    static void to_tuple(tuple_t& out, YAML::Node const& yaml) {
        using element_type = std::tuple_element_t<std::tuple_size<tuple_t>::value - 1 - index, tuple_t>;
        std::get<std::tuple_size<tuple_t>::value - 1 - index>(out) =
            ::yadi::from_yaml<element_type>(yaml[std::tuple_size<tuple_t>::value - 1 - index]);
        print_thing<tuple_t, index - 1>::to_tuple(out, yaml);
    }
};

template <typename tuple_t>
struct print_thing<tuple_t, 0> {
    static void print(YAML::Node const& yaml) {
        using element_type = std::tuple_element_t<std::tuple_size<tuple_t>::value - 1, tuple_t>;
        std::cout << ::yadi::from_yaml<element_type>(yaml[std::tuple_size<tuple_t>::value - 1]) << '\n';
    }

    static void to_tuple(tuple_t& out, YAML::Node const& yaml) {
        using element_type = std::tuple_element_t<std::tuple_size<tuple_t>::value - 1, tuple_t>;
        std::get<std::tuple_size<tuple_t>::value - 1>(out) =
            ::yadi::from_yaml<element_type>(yaml[std::tuple_size<tuple_t>::value - 1]);
    }
};

template <typename T>
void printer(T const&, YAML::Node const& yaml) {
    print_thing<T>::print(yaml);
}

template <typename T>
struct function_args_to_tuple;

template <typename R, typename... Args>
struct function_args_to_tuple<R (*)(Args...)> {
    using result_type = R;
    using params_type = std::tuple<Args...>;

    std::function<R(Args...)> func;
    std::tuple<Args...> params;

    template <std::size_t... I>
    R call_func(std::index_sequence<I...>) {
        return func(std::get<I>(params)...);
    }

    result_type delayed_dispatch() { return call_func(std::index_sequence_for<Args...>{}); }

    static result_type call(std::function<R(Args...)> const& func, YAML::Node const& yaml) {
        // Use std::apply in c++17
        params_type params;
        print_thing<params_type>::to_tuple(params, yaml);
        function_args_to_tuple f = {func, params};
        return f.delayed_dispatch();
    }
};

template <typename R, typename... Args>
struct function_args_to_tuple<std::function<R(Args...)>> {
    using result_type = R;
    using params_type = std::tuple<Args...>;

    std::function<R(Args...)> func;
    std::tuple<Args...> params;

    template <std::size_t... I>
    R call_func(std::index_sequence<I...>) {
        return func(std::get<I>(params)...);
    }

    result_type delayed_dispatch() { return call_func(std::index_sequence_for<Args...>{}); }

    static result_type call(std::function<R(Args...)> const& func, YAML::Node const& yaml) {
        // Use std::apply in c++17
        params_type params;
        print_thing<params_type>::to_tuple(params, yaml);
        function_args_to_tuple f = {func, params};
        return f.delayed_dispatch();
    }
};

template <typename T>
using function_args_to_tuple_params_type = typename function_args_to_tuple<T>::params_type;

template <typename T>
using function_args_to_tuple_result_type = typename function_args_to_tuple<T>::result_type;

template <typename F>
function_args_to_tuple_result_type<F> call_from_yaml(F const& func, YAML::Node const& yaml) {
    return function_args_to_tuple<F>::call(func, yaml);
}

template <typename base_t, typename F>
::yadi::initializer_type_t<base_t> make_initializer(F func) {
    return [func](YAML::Node const& yaml) { return call_from_yaml(func, yaml); };
};

double sum(double a1, double a2, int a3, double a4) { return a1 + a2 + a3 + a4; }

struct Foo {
    static double sum(double a1, double a2, int a3, double a4) { return a1 + a2 + a3 + a4; }
};

YADI_TEST(function_test) {
    std::string YAML = R"raw(
---
- 1.5
- 2.5
- 3
- 4.5
)raw";
    using ctr_sig = std::function<double(double, double, int, double)>;

    function_args_to_tuple_params_type<ctr_sig> test;
    printer(test, YAML::Load(YAML));
    print_thing<decltype(test)>::to_tuple(test, YAML::Load(YAML));
    YADI_ASSERT_EQ(std::get<0>(test), 1.5);
    YADI_ASSERT_EQ(std::get<1>(test), 2.5);
    YADI_ASSERT_EQ(std::get<2>(test), 3);
    YADI_ASSERT_EQ(std::get<3>(test), 4.5);

    {
        double result = call_from_yaml(&sum, YAML::Load(YAML));
        YADI_ASSERT_EQ(11.5, result);
    }

    {
        double result = call_from_yaml(&Foo::sum, YAML::Load(YAML));
        YADI_ASSERT_EQ(11.5, result);
    }

    {
        // create initializer type the factory will use...
        auto initializer = make_initializer<double>(&sum);
        YADI_ASSERT_EQ(11.5, initializer(YAML::Load(YAML)));
    }

    return true;
}
