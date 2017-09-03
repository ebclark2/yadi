#include <iostream>
#include "yadi/yadi.hpp"

int main() {
    int i = ::yadi::from_yaml<int>(YAML::Load("1"));
    return i - 1;
}
