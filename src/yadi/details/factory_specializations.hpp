//
// Created by Ed Clark on 8/29/17.
//

#ifndef YADI_FACTORY_SPECIALIZATIONS_HPP
#define YADI_FACTORY_SPECIALIZATIONS_HPP

#include "create_utils.hpp"
#include "factory.hpp"

#include <vector>

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

    // TODO Should config default value be removed?
    // TODO Should this be opened up for creating aliases?
    // TODO Should this just be a specialization of yadi::create?
    // TODO Should the yaml binding initializer just call from_yamls?
    // TODO Should some other create helper thing be created so we don't have to specialize factory to these
    // shinanigans?
    // TODO Perhaps we should check to see if a factory is used for said type and if not check to see if there is a
    // create helper impl?
    // TODO Some sort of yaml binding helper must be made.  This specialization is bad.
    // TODO First we need to be able to indicate a factory has been explicitly used for some type
    static ptr_type create(std::string const& /*type*/, YAML::Node const& config = {}) {
        std::vector<T> ret;
        from_yamls<derive_base_type_t<T>>(config, std::back_inserter(ret));
        return ret;
    }
};

}  // namespace yadi

#endif  // YADI_FACTORY_SPECIALIZATIONS_HPP
