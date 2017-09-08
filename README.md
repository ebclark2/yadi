# YADI ![Build Status](https://travis-ci.org/ebclark2/yadi.svg?branch=master)
YADI aims to provide non-intrusive dependency injection via human readable YAML config with generated help information.  

Example help information, see test/yadi/example.cpp

```text
double
	"type_by_value" -> Direct conversion using yaml.as<double>()
float
	"type_by_value" -> Direct conversion using yaml.as<float>()
int
	"type_by_value" -> Direct conversion using yaml.as<int>()
std::string
	"type_by_value" -> Direct conversion using yaml.as<std::string>()
yadi::car
	"type_by_value" -> Expects yaml map with fields:
		make: std::string, The make of the car!
		power_plant: yadi::power_plant, What makes the car go!
yadi::power_plant
	"electric" -> Expects yaml sequence with types:
		 - std::string, Origin of motor
		 - int, Watts
		 - list<int>, Random numbers for testing?
	"gas" -> Expects yaml map with fields:
		make: std::string, Engine make
		cylinder_count: int, The number of cylinders
		bore: float, Cylidner bore in inches
		stroke: float, Cylinder stroke in inches
		vendors: set<std::string>, Vendors providing parts for engine
```
Example configuration
```yaml

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
```
Example classes
```
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
```
### Build requirements
 - c++14
 - conan.io, see http://conan.io
 - edsidea conan remote, `conan remote add <REMOTE> https://api.bintray.com/conan/edsidea/edsidea` where `<REMOTE>` is your name for this remote

### Building from repository
 - mkdir build
 - cd build
 - conan install .. (`conan install .. --build yaml-cpp` perhaps)
 - conan build ..
 - ctest

### Definitions
 - base type: The type a factory instantiates.  factory<foo> would have a base type of foo.
 - ptr type: The type created by a factory.  The default pointer type is std::unique_ptr<base_type>.  Returning by value is possible but implies no inheritance is used.  This name is poor and will change soon.
 - output type: The return type of a create function.  By default the base types pointer type is used, but other types may be requested.  Anything output type that's true for std::is_convertible<output_type, base_type> is allowed and adapters may facilitate other conversions. 
 - initializer: A function capable of instantiaing the base type of a factory.
 - adapter: A layer on top of factory<foo>::create to allow types such as templated containers to be created without having to register initializers for each element type.
 - type: A string identifying an initializer.
 - config: YAML to be passed to an initializer.
 - factory config: YAML containing both type information and config to be passed to an initializer. 
