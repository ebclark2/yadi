//
// Created by Ed Clark on 8/26/17.
//

#include <yadi/yadi.hpp>

#include <iostream>

int main() {
    using namespace yadi;
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
