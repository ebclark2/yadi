//
// Created by Ed Clark on 8/21/17.
//

#include <yadi/std_types.hpp>
#include "test.hpp"

#include <functional>
#include <iostream>

/*
 * Experiments that hopefully lead to building a tuple from a YAML sequence based on the param pack
 * of a std function. This could then be unpacked to call some function giving a binding
 * implementation of sorts, so long as I don't forget what I'm doing.
 */

template <size_t N, typename Tuple_Type>
struct tuple_trunc {};

template <size_t N, typename Head, typename... Tail>
struct tuple_trunc<N, std::tuple<Head, Tail...>> {
    typedef typename tuple_trunc<N - 1, std::tuple<Tail...>>::type type;
};

template <typename Head, typename... Tail>
struct tuple_trunc<0, std::tuple<Head, Tail...>> {
    typedef std::tuple<Head, Tail...> type;
};

template <size_t N, size_t I>
struct assign {
    template <class ResultTuple, class SrcTuple>
    static void x(ResultTuple& t, const SrcTuple& tup) {
        std::get<I - N>(t) = std::get<I>(tup);
        assign<N, I - 1>::x(t, tup);  // this offsets the assignment index by N
    }
};

template <size_t N>
struct assign<N, 1> {
    template <class ResultTuple, class SrcTuple>
    static void x(ResultTuple& t, const SrcTuple& tup) {
        std::get<0>(t) = std::get<1>(tup);
    }
};

template <size_t TruncSize, class Tup>
struct th2;

template <size_t TruncSize, class Head, class... Tail>
struct th2<TruncSize, std::tuple<Head, Tail...>> {
    typedef typename tuple_trunc<TruncSize, std::tuple<Head, Tail...>>::type type;

    static type tail(const std::tuple<Head, Tail...>& tup) {
        type t;
        assign<TruncSize, std::tuple_size<type>::value>::x(t, tup);
        return t;
    }
};

template <size_t TruncSize, class Tup>
typename th2<TruncSize, Tup>::type tail(const Tup& tup) {
    return th2<TruncSize, Tup>::tail(tup);
}

template <typename tuple_t, size_t index = std::tuple_size<tuple_t>::value - 1>
struct print_thing {
    static void print(YAML::Node const& yaml) {
        using element_type =
            std::tuple_element_t<std::tuple_size<tuple_t>::value - 1 - index, tuple_t>;
        // std::cout << typeid(std::tuple_element_t<std::tuple_size<tuple_t>::value-1-index,
        // tuple_t>).name() << '\n';
        //        std::cout << yaml[std::tuple_size<tuple_t>::value-1-index].template
        //        as<std::tuple_element_t<std::tuple_size<tuple_t>::value-1-index, tuple_t>>() <<
        //        '\n';
        std::cout << ::yadi::from_yaml<element_type>(
                         yaml[std::tuple_size<tuple_t>::value - 1 - index])
                  << '\n';
        print_thing<tuple_t, index - 1>::print(yaml);
    }
};

template <typename tuple_t>
struct print_thing<tuple_t, 0> {
    static void print(YAML::Node const& yaml) {
        using element_type = std::tuple_element_t<std::tuple_size<tuple_t>::value - 1, tuple_t>;
        // std::cout << typeid(std::tuple_element_t<std::tuple_size<tuple_t>::value-1,
        // tuple_t>).name() << '\n';  std::cout << yaml[std::tuple_size<tuple_t>::value-1].template
        // as<std::tuple_element_t<std::tuple_size<tuple_t>::value-1, tuple_t>>() << '\n';
        std::cout << ::yadi::from_yaml<element_type>(yaml[std::tuple_size<tuple_t>::value - 1])
                  << '\n';
    }
};

template <typename T>
void printer(T const&, YAML::Node const& yaml) {
    print_thing<T>::print(yaml);
}
YADI_TEST(function_test) {
    std::string YAML = R"raw(
---
- 1.5
- 2.5
- 3
- 4.5
)raw";
    std::tuple<double, double, int, double> test(1, 2, 3, 4);
    // tuple_trunc<2, std::tuple<double, double, int, double>>::type c = tail<2>(test);
    printer(test, YAML::Load(YAML));
    return true;
}
