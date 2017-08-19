#ifndef YADI_FACTORY_HPP__
#define YADI_FACTORY_HPP__

#include <yaml-cpp/yaml.h>

#include <functional>
#include <map>
#include <memory>
#include <string>

namespace yadi {

template <typename base_t> struct factory_traits {
  using ptr_type = std::shared_ptr<base_t>;
};

template <typename base_t>
using ptr_type_t = typename factory_traits<base_t>::ptr_type;

template <typename base_t> class factory {
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

  template <typename impl_t> static void register_type(std::string type) {
    register_type(type, [](YAML::Node) {
      ptr_type_t<base_type> p(new impl_t);
      return p;
    });
  }

  static ptr_type create(std::string const &type, YAML::Node const& config = {}) {
    typename type_store::const_iterator type_iter = mut_types().find(type);
    if (type_iter == mut_types().end()) {
      throw std::runtime_error(type + " not found");
    }

    return type_iter->second.initializer(config);
  }

    static type_store types() {
        return mut_types();
    }

private:
  static type_store &mut_types() {
    static type_store TYPES;
    return TYPES;
  }
};

    template <typename base_t>
    using initializer_type_t = typename factory<base_t>::initializer_type;

    template <typename base_t>
    class registrator
    {
    public:
        registrator(std::string type, initializer_type_t<base_t> initializer)
        {
            factory<base_t>::register_type(type, initializer);
        }
    };

    template <typename base_t, typename impl_t>
    class registrator_no_arg
    {
    public:
        registrator_no_arg(std::string type)
        {
            factory<base_t>::template register_type<impl_t>(type);
        }
    };
    
} // namespace yadi

#endif // YADI_FACTORY_HPP__
