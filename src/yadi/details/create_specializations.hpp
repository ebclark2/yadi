//
// Created by Ed Clark on 8/29/17.
//

#ifndef YADI_FACTORY_SPECIALIZATIONS_HPP
#define YADI_FACTORY_SPECIALIZATIONS_HPP

#include "create_utils.hpp"
#include "factory.hpp"

#include <iterator>
#include <list>
#include <vector>

namespace yadi {

template <typename LT, typename ET>
struct back_inserter_adapter {
    using output_type = LT;
    static bool const direct_from_yaml = true;

    static output_type create(std::string const&, YAML::Node const& config = {}) {
        output_type out;
        from_yamls<ET>(config, std::back_inserter(out));
        return out;
    }
};

template <typename ST, typename ET>
struct inserter_adapter {
    using output_type = ST;
    static bool const direct_from_yaml = true;

    static output_type create(std::string const&, YAML::Node const& config = {}) {
        output_type out;
        from_yamls<ET>(config, std::inserter(out, out.end()));
        return out;
    }
};

template <typename ET>
struct adapter<std::list<ET>, std::list<ET>> : public back_inserter_adapter<std::list<ET>, ET> {};

template <typename ET>
struct adapter<std::vector<ET>, std::vector<ET>> : public back_inserter_adapter<std::vector<ET>, ET> {};

template <typename ET>
struct adapter<std::set<ET>, std::set<ET>> : public inserter_adapter<std::set<ET>, ET> {};

}  // namespace yadi

#endif  // YADI_FACTORY_SPECIALIZATIONS_HPP
