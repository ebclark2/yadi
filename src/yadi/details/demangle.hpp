//
// Created by Ed Clark on 8/29/17.
//

#ifndef YADI_DEMANGLE_HPP
#define YADI_DEMANGLE_HPP

#include <string>
#include <typeinfo>

namespace yadi {

std::string demangle(const char* name);

template <typename T>
std::string demangle_type() {
    return demangle(typeid(T).name());
}

}  // namespace yadi

#endif  // YADI_DEMANGLE_HPP
