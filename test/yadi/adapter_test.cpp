//
// Created by Ed Clark on 8/31/17.
//

#include "test.hpp"

#include <list>
#include <type_traits>

namespace yadi {

template <typename FT, typename OT = ptr_type_t<derive_base_type_t<FT>>>
struct adapter;

template <typename FT>
struct adapter<FT, ptr_type_t<derive_base_type_t<FT>>> {
    using factory_type = derive_base_type_t<FT>;
    using output_type = ptr_type_t<derive_base_type_t<FT>>;

    static output_type create(std::string const& type, YAML::Node const& config = {}) {
        return factory<factory_type>::create(type, config);
    }
};

template <typename ET>
struct adapter<std::list<ET>, std::list<ET>> {
    using output_type = std::list<ET>;

    static output_type create(std::string const&, YAML::Node const& config = {}) {
        output_type out;
        from_yamls<ET>(config, std::back_inserter(out));
        return out;
    }
};

YADI_TEST(adapter_test) {
    int five = adapter<int>::create(type_by_value_key(), YAML::Load("5"));
    YADI_ASSERT_EQ(5, five);

    std::list<int> fives = adapter<std::list<int>, std::list<int>>::create("", YAML::Load("[5, 5, 5, 5, 5]"));
    YADI_ASSERT_EQ(5, fives.size());

    return true;
}

}  // namespace yadi