//
// Created by Ed Clark on 8/29/17.
//

#include "create_utils.hpp"

std::string const& yadi::type_by_value_key() {
    static std::string const TYPE_BY_VALUE = "type_by_value";
    return TYPE_BY_VALUE;
}
