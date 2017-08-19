//
// Created by Ed Clark on 8/18/17.
//

#include "test.hpp"

#include <iomanip>
#include <iostream>

int main() {
  using namespace yadi;

  bool success = true;

  test::type_store types = test::types();
  for (auto const& entry : types) {
    std::string const& type = entry.first;
    ptr_type_t<test> t = test::create(type);
    std::cout << "[" << type << "] running...\n";
    bool result = t->run();
    success = success && result;
    std::cout << "[" << type << "] " << (result ? "passed" : "failed")
              << std::endl;
  }

  return !success;
}