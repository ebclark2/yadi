//
// Created by Ed Clark on 8/29/17.
//

#include "help.hpp"

namespace yadi {

yadi_help_fetcher::yadi_help_fetcher() {}
yadi_help_fetcher::yadi_help_fetcher(yadi_help_fetcher const& other) : impl(other.impl->clone().release()) {}

yadi_help_fetcher& yadi_help_fetcher::operator=(yadi_help_fetcher const& other) {
    this->impl = other.impl->clone();
    return *this;
}

yadi_help::help_store const& yadi_help::helps() { return mut_helps(); }
yadi_help::name_store const& yadi_help::names() { return mut_names(); }

yadi_help::help_store& yadi_help::mut_helps() {
    static help_store HELPS;
    return HELPS;
}

yadi_help::name_store& yadi_help::mut_names() {
    static name_store NAMES;
    return NAMES;
}

}  // namespace yadi
