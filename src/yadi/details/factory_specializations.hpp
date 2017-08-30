//
// Created by Ed Clark on 8/29/17.
//

#ifndef YADI_FACTORY_SPECIALIZATIONS_HPP
#define YADI_FACTORY_SPECIALIZATIONS_HPP

#include "create_utils.hpp"
#include "factory.hpp"

namespace yadi {

template <typename T>
struct factory_traits<std::vector<T>> {
    using ptr_type = std::vector<T>;
    static const bool direct_from_yaml = true;
};

template <typename T>
class factory<std::vector<T>> {
   public:
    using base_type = std::vector<T>;
    using initializer_type = std::function<ptr_type_t<base_type>(YAML::Node)>;
    using ptr_type = ptr_type_t<base_type>;

    static ptr_type create(std::string const& /*type*/, YAML::Node const& config = {}) {
        std::vector<T> ret;
        from_yamls<T>(config, std::back_inserter(ret));
        return ret;
    }
};

}  // namespace yadi

#endif  // YADI_FACTORY_SPECIALIZATIONS_HPP
