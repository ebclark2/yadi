#ifndef YADI_FACTORY_HPP__
#define YADI_FACTORY_HPP__

#include <yaml-cpp/yaml.h>

#include <functional>
#include <map>
#include <memory>
#include <string>

namespace yadi {

template <typename base_t>
struct factory_traits {
  using ptr_type = std::shared_ptr<base_t>;
};

template <typename base_t>
using ptr_type_t = typename factory_traits<base_t>::ptr_type;

template <typename base_t>
class factory {
 public:
  using base_type = base_t;
  using initializer_type = std::function<ptr_type_t<base_type>(YAML::Node)>;
  using ptr_type = ptr_type_t<base_type>;

 private:
  struct type_info {
    initializer_type initializer;
    std::string help;
  };

 public:
  using type_store = std::map<std::string, type_info>;

 public:
  static void register_type(std::string type, initializer_type initializer) {
    mut_types()[type].initializer = initializer;
  }

  static ptr_type create(std::string const& type,
                         YAML::Node const& config = {}) {
    typename type_store::const_iterator type_iter = mut_types().find(type);
    if (type_iter == mut_types().end()) {
      throw std::runtime_error(type + " not found");
    }

    return type_iter->second.initializer(config);
  }

  static ptr_type create(YAML::Node const& factory_config) {
    if (!factory_config.IsDefined()) {
      throw std::runtime_error("Factory config not defined");
    }

    if (factory_config.IsScalar()) {
      std::string type = factory_config.as<std::string>("");
      if (type.empty()) {
        throw std::runtime_error("Factory config scalar not valid");
      }

      return create(type);
    }

    if (factory_config.IsMap()) {
      YAML::Node typeNode = factory_config["type"];
      if (!typeNode.IsDefined()) {
        throw std::runtime_error("Factory config type not defined");
      }
      std::string type = typeNode.as<std::string>("");
      if (type.empty()) {
        throw std::runtime_error("Factory config type not valid");
      }

      YAML::Node configNode = factory_config["config"];
      return create(type, configNode);
    }

    throw std::runtime_error(
        "Factory config not valid, YAML must be scalar string or map");
  }

  static type_store types() { return mut_types(); }

 private:
  static type_store& mut_types() {
    static type_store TYPES;
    return TYPES;
  }
};

template <typename base_t>
using initializer_type_t = typename factory<base_t>::initializer_type;

template <typename base_t, typename impl_t>
ptr_type_t<base_t> yaml_init(YAML::Node const& config) {
  ptr_type_t<base_t> ret(new impl_t(config));
  return ret;
};

template <typename base_t>
static void register_type(std::string type,
                          initializer_type_t<base_t> initializer) {
  factory<base_t>::register_type(type, initializer);
};

template <typename base_t, typename impl_t>
static void register_type(std::string type) {
  register_type<base_t>(type, &yaml_init<base_t, impl_t>);
};

template <typename base_t, typename impl_t>
static void register_type_no_arg(std::string type) {
  register_type<base_t>(type, [](YAML::Node) {
    ptr_type_t<base_t> p(new impl_t);
    return p;
  });
}

}  // namespace yadi

#define YADI_INIT_BEGIN_N(NAME)         \
  namespace {                           \
  struct static_initialization_##NAME { \
    static_initialization_##NAME() {
#define YADI_INIT_END_N(NAME)                                    \
  }                                                              \
  }                                                              \
  ;                                                              \
  static_initialization_##NAME static_initialization_##NAME##__; \
  }

#define YADI_INIT_BEGIN YADI_INIT_BEGIN_N(ANON)
#define YADI_INIT_END YADI_INIT_END_N(ANON)

#endif  // YADI_FACTORY_HPP__
