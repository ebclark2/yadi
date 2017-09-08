//
// Created by Ed Clark on 8/20/17.
//

#include "test.hpp"

#include <yadi/yadi.hpp>

#include <iterator>
#include <vector>

using namespace std::string_literals;

namespace yadi {

struct power_plant {
    virtual ~power_plant() {}

    virtual int power() const = 0;
};

struct electric : public power_plant {
    electric(std::string make, int watts, std::vector<int> numbers)
        : make(std::move(make)), watts(std::move(watts)), numbers(numbers) {}

    int power() const { return this->watts; }

    std::string make;
    int watts;
    std::vector<int> numbers;
};

struct gas : public power_plant {
    static std::unique_ptr<power_plant> make_gas(std::string make, int cylinder_count, float bore, float stroke,
                                                 std::set<std::string> vendors) {
        std::unique_ptr<power_plant> ret(new gas(make, cylinder_count, bore, stroke, vendors));
        return ret;
    }

    gas(std::string make, int cylinder_count, float bore, float stroke, std::set<std::string> vendors)
        : make(make), cylinder_count(cylinder_count), bore(bore), stroke(stroke), vendors(std::move(vendors)) {}

    int power() const { return this->bore * this->stroke * this->cylinder_count; }

    std::string make;
    int cylinder_count;
    float bore;
    float stroke;
    std::set<std::string> vendors;
};

struct car {
    car(std::string make, std::unique_ptr<power_plant>&& motor) : make(std::move(make)), motor(std::move(motor)) {}

    std::string make;
    std::unique_ptr<power_plant> motor;
};

template <>
struct factory_traits<car> {
    using ptr_type = car;
    static const bool direct_from_yaml = true;
};

YADI_INIT_BEGIN
::yadi::register_factory<car>();
::yadi::register_factory<power_plant>();
// Make car from mapped args
::yadi::register_type<car>(
    ::yadi::type_by_value_key(),
    ::yadi::make_map_initializer_with_help<car>(&ctr_init<car, car, std::string, std::unique_ptr<power_plant>>,
                                                {std::make_pair("make"s, "The make of the car!"s),
                                                 std::make_pair("power_plant"s, "What makes the car go!"s)}));

// Make gas from mapped args
register_type<power_plant>(
    "gas",
    ::yadi::make_map_initializer_with_help<power_plant>(
        &ctr_init<power_plant, gas, std::string, int, float, float, std::set<std::string>>,
        {std::make_pair("make", "Engine make"), std::make_pair("cylinder_count", "The number of cylinders"),
         std::make_pair("bore", "Cylidner bore in inches"), std::make_pair("stroke", "Cylinder stroke in inches"),
         std::make_pair("vendors", "Vendors providing parts for engine")}));
// Make electric from sequenced args
register_type<power_plant>("electric", ::yadi::make_sequence_initializer_with_help<power_plant>(
                                           &ctr_init<power_plant, electric, std::string, int, std::vector<int>>,
                                           {"Origin of motor", "Watts", "Random numbers for testing?"}));
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
      bore: 3.72
      stroke: 3.44
      vendors:
        - Currie
        - Synergy
        - Some other vendor
- make: "tesla"
  power_plant:
    type: electric
    config:
      - USA
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
        YADI_ASSERT_EQ(372, (int)(pp.bore * 100.0f));
        YADI_ASSERT_EQ(344, (int)(pp.stroke * 100.0f));
        YADI_ASSERT_EQ(3, pp.vendors.size());
    }
    {
        car const& c = cars[1];
        YADI_ASSERT_EQ("tesla", c.make);
        electric const& pp = dynamic_cast<electric const&>(*c.motor);
        YADI_ASSERT_EQ("USA", pp.make);
        YADI_ASSERT_EQ(1200, pp.watts);
        std::vector<int> expectedNumbers = {1, 2, 3, 4, 5};
        YADI_ASSERT_EQ(expectedNumbers, pp.numbers);
    }
    return true;
}

}  // namespace yadi