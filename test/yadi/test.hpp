//
// Created by Ed Clark on 8/18/17.
//

#ifndef YADI_TEST_HPP
#define YADI_TEST_HPP

#include <yadi/factory.hpp>

namespace yadi {
class test;

template <>
struct factory_traits<test> {
    using ptr_type = std::unique_ptr<test>;
};

class test : public factory<test> {
   public:
    virtual bool run() = 0;
};
}  // namespace yadi

#define YADI_TEST(NAME) YADI_TEST_(NAME)
#define YADI_TEST_(NAME)                                     \
    class NAME : public ::yadi::test {                       \
       public:                                               \
        bool run() override final;                           \
    };                                                       \
    YADI_INIT_BEGIN_N(NAME)                                  \
    ::yadi::register_type_no_arg<::yadi::test, NAME>(#NAME); \
    YADI_INIT_END_N(NAME)                                    \
    bool NAME::run()

#endif  // YADI_TEST_HPP
