//
// Created by Ed Clark on 8/20/17.
//

#include "test.hpp"

#include <yadi/yadi.hpp>

#include <iterator>
#include <vector>

namespace yadi {

struct power_plant {
    virtual ~power_plant() {}

    virtual int power() const = 0;
};

struct electric : public power_plant {
    static std::unique_ptr<power_plant> make_electric(std::string make, int watts, std::vector<int> numbers) {
        std::unique_ptr<power_plant> ret(new electric(make, watts, numbers));
        return ret;
    }
    electric(std::string make, int watts, std::vector<int> numbers)
        : make(std::move(make)), watts(std::move(watts)), numbers(numbers) {}

    int power() const { return this->watts; }

    std::string make;
    int watts;
    std::vector<int> numbers;
};

struct gas : public power_plant {
    static std::unique_ptr<power_plant> make_gas(std::string make, int cylinder_count, int bore, int stroke) {
        std::unique_ptr<power_plant> ret(new gas(make, cylinder_count, bore, stroke));
        return ret;
    }

    gas(std::string make, int cylinder_count, int bore, int stroke)
        : make(make), cylinder_count(cylinder_count), bore(bore), stroke(stroke) {}

    int power() const { return this->bore * this->stroke * this->cylinder_count; }

    std::string make;
    int cylinder_count;
    int bore;
    int stroke;
};

struct car {
    static car make_car(std::string make, std::unique_ptr<power_plant>& motor) {
        return car{std::move(make), std::move(motor)};
    }
    car(std::string make, std::unique_ptr<power_plant> motor) : make(std::move(make)) { this->motor.swap(motor); }

    std::string make;
    std::unique_ptr<power_plant> motor;
};

template <>
struct factory_traits<car> {
    using ptr_type = car;
    static const bool direct_from_yaml = true;
};

YADI_INIT_BEGIN
::yadi::yadi_help::register_factory<car>("car");
::yadi::yadi_help::register_factory<power_plant>("power_plant");
// Make car from mapped args
::yadi::register_type<car>("", ::yadi::make_initializer<car>(&car::make_car, {"make", "power_plant"}));

// Make gas from mapped args
register_type<power_plant>("gas", ::yadi::make_initializer<power_plant>(&gas::make_gas,
                                                                        {"make", "cylinder_count", "bore", "stroke"}));
// Make electric from sequenced args
register_type<power_plant>("electric", ::yadi::make_initializer<power_plant>(&electric::make_electric));
YADI_INIT_END

YADI_TEST(nested_example_test) {
    std::string YAML_CONFIG = R"raw(
---
- make: "gm"
  power_plant:
    type: gas
    config:
      make: LQ4
      cylinder_count: 8
      bore: 1
      stroke: 8
- make: "tesla"
  power_plant:
    type: electric
    config:
      - japan
      - 1200
      - - 1
        - 2
        - 3
        - 4
        - 5
)raw";

    std::vector<car> cars = from_yaml<std::vector<car>>(YAML::Load(YAML_CONFIG));
    // from_yamls<car>(YAML::Load(YAML_CONFIG), std::back_inserter(cars));

    YADI_ASSERT_EQ(2u, cars.size());
    {
        car const& c = cars[0];
        YADI_ASSERT_EQ("gm", c.make);
        gas const& pp = dynamic_cast<gas const&>(*c.motor);
        YADI_ASSERT_EQ("LQ4", pp.make);
        YADI_ASSERT_EQ(8, pp.cylinder_count);
        YADI_ASSERT_EQ(1, pp.bore);
        YADI_ASSERT_EQ(8, pp.stroke);
    }
    {
        car const& c = cars[1];
        YADI_ASSERT_EQ("tesla", c.make);
        electric const& pp = dynamic_cast<electric const&>(*c.motor);
        YADI_ASSERT_EQ("japan", pp.make);
        YADI_ASSERT_EQ(1200, pp.watts);
        std::vector<int> expectedNumbers = {1, 2, 3, 4, 5};
        YADI_ASSERT_EQ(expectedNumbers, pp.numbers);
    }
    return true;
}

}  // namespace yadi