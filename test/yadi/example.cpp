//
// Created by Ed Clark on 8/20/17.
//

#include "test.hpp"

#include <yadi/std_types.hpp>
#include <yadi/yadi.hpp>

#include <iterator>
#include <list>

namespace yadi {

struct power_plant {
    virtual ~power_plant() {}

    virtual int power() const = 0;
};

struct electric : public power_plant {
    electric(YAML::Node const& config) {
        ::yadi::parse(this->make, config["make"]);
        ::yadi::parse(this->watts, config["watts"]);
    }

    int power() const { return this->watts; }

    std::string make;
    int watts;
};

struct gas : public power_plant {
    gas(YAML::Node const& config) {
        ::yadi::parse(this->make, config["make"]);
        ::yadi::parse(this->cylinder_count, config["cylinder_count"]);
        ::yadi::parse(this->bore, config["bore"]);
        ::yadi::parse(this->stroke, config["stroke"]);
    }

    int power() const { return this->bore * this->stroke * this->cylinder_count; }

    std::string make;
    int cylinder_count;
    int bore;
    int stroke;
};

struct car {
    car(YAML::Node const& config) {
        ::yadi::parse(this->make, config["make"]);
        ::yadi::parse(this->motor, config["motor"]);
    }

    std::string make;
    std::unique_ptr<power_plant> motor;
};

template <>
struct factory_traits<car> {
    using ptr_type = car;
    static const bool direct_from_yaml = true;
};

YADI_INIT_BEGIN
::yadi::register_type<car, car>("");
register_type<power_plant, gas>("gas");
register_type<power_plant, electric>("electric");
YADI_INIT_END

YADI_TEST(nested_example_test) {
    std::string YAML_CONFIG = R"raw(
---
- make: gm
  motor:
    type: gas
    config:
      make: LQ4
      cylinder_count: 8
      bore: 10
      stroke: 8
- make: tesla
  motor:
    type: electric
    config:
      make: japan
      watts: 1200
)raw";

    std::list<car> cars;
    from_yamls<car>(YAML::Load(YAML_CONFIG), std::back_inserter(cars));

    YADI_ASSERT_EQ(2u, cars.size());
    return true;
}

}  // namespace yadi