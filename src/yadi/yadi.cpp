//
// Created by Ed Clark on 8/28/17.
//

#include "yadi.hpp"

#ifdef __GNUG__
#include <cxxabi.h>
#include <cstdlib>
#include <memory>
#endif

namespace yadi {

#ifdef __GNUG__

std::string demangle(const char* name) {
    int status = -4;  // some arbitrary value to eliminate the compiler warning

    // enable c++11 by passing the flag -std=c++11 to g++
    std::unique_ptr<char, void (*)(void*)> res{abi::__cxa_demangle(name, NULL, NULL, &status), std::free};

    return (status == 0) ? res.get() : name;
}

#else

// does nothing if not g++
std::string demangle(const char* name) { return name; }

#endif

yadi_help_fetcher::yadi_help_fetcher() {}
yadi_help_fetcher::yadi_help_fetcher(yadi_help_fetcher const& other) : impl(other.impl->clone().release()) {}

yadi_help_fetcher& yadi_help_fetcher::operator=(yadi_help_fetcher const& other) {
    this->impl = other.impl->clone();
    return *this;
}

yadi_help::help_store const& yadi_help::helps() { return mut_helps(); }

yadi_help::help_store& yadi_help::mut_helps() {
    static help_store HELPS;
    return HELPS;
}

}  // namespace yadi
