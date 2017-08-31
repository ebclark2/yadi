//
// Created by Ed Clark on 8/30/17.
//

#include "test.hpp"

#include <yadi/inspector.hpp>

#include <iostream>

YADI_TEST(inspector_test) {
    ::yadi::print_factory_help(std::cout);

    return true;
}