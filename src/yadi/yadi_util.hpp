//
// Created by Ed Clark on 8/26/17.
//

#ifndef YADI_YADI_UTIL_HPP
#define YADI_YADI_UTIL_HPP

#include "yadi_help.hpp"

#include <iostream>

namespace yadi {

int main() {
    yadi_help::help_store helps = yadi_help::helps();
    for (auto const& entry : helps) {
        std::cout << entry.first << '\n';
        std::vector<std::string> types = entry.second.getTypes();
        for (std::string const& type : types) {
            std::cout << "\t\"" << type << "\" -> " << entry.second.getHelp(type) << '\n';
        }
    }

    return 0;
}

}  // namespace yadi

int main() { return ::yadi::main(); }

#endif  // YADI_YADI_UTIL_HPP
