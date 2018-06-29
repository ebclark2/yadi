//
// Created by Ed Clark on 8/30/17.
//
#include "initializers.hpp"

namespace yadi {

std::string formatFields(std::vector<std::string> const &fields) {
    std::string fieldsStr{"[ "};
    for(auto const& f : fields) {
        fieldsStr += (f + " ");
    }

    return fieldsStr += "]";
}

} // end namespace yadi

