//
// Created by Ed Clark on 8/30/17.
//

#include "registration.hpp"

YAML::Node yadi::merge_yaml(YAML::Node const& left, YAML::Node const& right) {
    static std::string const YAML_TYPE_NAMES[] = {"Undefined", "Null", "Scalar", "Sequence", "Map"};

    if (!right.IsDefined()) {
        return left;
    }
    if (!left.IsDefined()) {
        return right;
    }

    if (left.IsMap() && right.IsMap()) {
        YAML::Node ret = YAML::Clone(right);
        for (auto map_iter : left) {
            ret.force_insert(YAML::Clone(map_iter.first), YAML::Clone(map_iter.second));
        }
        return ret;
    }

    throw std::runtime_error("Unable to merge YAML types " + YAML_TYPE_NAMES[left.Type()] + " and " +
                             YAML_TYPE_NAMES[right.Type()]);
}
