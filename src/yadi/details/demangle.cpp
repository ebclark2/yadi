//
// Created by Ed Clark on 8/29/17.
//

#include "demangle.hpp"

#define YADI_HAS_DEMANGLE

#ifdef YADI_HAS_DEMANGLE
#include <cxxabi.h>
#include <cstdlib>
#include <memory>
#endif

namespace yadi {

#ifdef YADI_HAS_DEMANGLE

std::string demangle(const char* name) {
    int status = -1;
    std::unique_ptr<char, void (*)(void*)> res{abi::__cxa_demangle(name, NULL, NULL, &status), std::free};

    return (status == 0) ? res.get() : name;
}

#else

std::string demangle(const char* name) { return name; }

#endif

}  // namespace yadi