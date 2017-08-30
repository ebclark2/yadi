//
// Created by Ed Clark on 8/28/17.
//

#include "yadi.hpp"

namespace yadi {

std::string const TYPE_BY_VALUE = "type_by_value";

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
