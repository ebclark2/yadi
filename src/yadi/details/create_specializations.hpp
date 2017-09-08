//
// Created by Ed Clark on 8/29/17.
//
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#ifndef YADI_FACTORY_SPECIALIZATIONS_HPP
#define YADI_FACTORY_SPECIALIZATIONS_HPP

#include "create_utils.hpp"
#include "factory.hpp"

#include <iterator>
#include <list>
#include <vector>

namespace yadi {

template <typename FT>
struct adapter<FT, ptr_type_t<meta::derive_base_type_t<FT>>> {
    using base_type = meta::derive_base_type_t<FT>;
    using output_type = ptr_type_t<meta::derive_base_type_t<FT>>;
    static bool const direct_from_yaml = factory_traits<base_type>::direct_from_yaml;

    static output_type create(std::string const& type, YAML::Node const& config = {}) {
        return factory<base_type>::create(type, config);
    }

    static std::string get_name() { return yadi_help::get_name<base_type>(); }
};

template <typename LT, typename ET>
struct back_inserter_adapter {
    using base_type = meta::derive_base_type_t<ET>;
    using output_type = LT;
    static bool const direct_from_yaml = true;

    static output_type create(std::string const&, YAML::Node const& config = {}) {
        output_type out;
        from_yamls<ET>(config, std::back_inserter(out));
        return out;
    }

    static std::string get_name() {
        using namespace std::string_literals;
        return "list<"s + adapter<ET>::get_name() + ">";
    }
};

template <typename ST, typename ET>
struct inserter_adapter {
    using base_type = meta::derive_base_type_t<ET>;
    using output_type = ST;
    static bool const direct_from_yaml = true;

    static output_type create(std::string const&, YAML::Node const& config = {}) {
        output_type out;
        from_yamls<ET>(config, std::inserter(out, out.end()));
        return out;
    }

    static std::string get_name() {
        using namespace std::string_literals;
        return "set<"s + adapter<ET>::get_name() + ">";
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
#endif  // DOXYGEN_SHOULD_SKIP_THIS