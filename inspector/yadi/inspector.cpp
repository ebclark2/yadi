//
// Created by Ed Clark on 8/26/17.
//

#include "inspector.hpp"

#include <yadi/yadi.hpp>

void yadi::print_factory_help(std::ostream& out) {
    yadi_help::help_store helps = yadi_help::helps();
    for (auto const& entry : helps) {
        out << entry.first << '\n';
        std::vector<std::string> types = entry.second.getTypes();
        for (std::string const& type : types) {
            out << "\t\"" << type << "\" -> " << entry.second.getHelp(type) << '\n';
        }
    }
}
