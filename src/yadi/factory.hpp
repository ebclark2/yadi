#ifndef YADI_FACTORY_HPP__
#define YADI_FACTORY_HPP__

#include <functional>
#include <map>
#include <memory>
#include <string>

namespace yadi {

template <typename BaseType> struct factory_traits {
  using ptr_type = std::shared_ptr<BaseType>;
};

template <typename BaseType>
using ptr_type_t = typename factory_traits<BaseType>::ptr_type;

template <typename BaseType> class factory {
public:
  using base_type = BaseType;
  using initializer_type = std::function<ptr_type_t<base_type>(std::string)>;
  using ptr_type = ptr_type_t<base_type>;

private:
  struct type_info {
    initializer_type initializer;
    std::string help;
  };

  using type_store = std::map<std::string, type_info>;

public:
  static void register_type(std::string type, initializer_type initializer) {
    types()[type].initializer = initializer;
  }

  template <typename ImplType> static void register_type(std::string type) {
    register_type(type, [](std::string) {
      ptr_type_t<base_type> p(new ImplType);
      return p;
    });
  }

  static ptr_type create(std::string const &type, std::string arg = "") {
    typename type_store::const_iterator type_iter = types().find(type);
    if (type_iter == types().end()) {
      throw std::runtime_error(type + " not found");
    }

    return type_iter->second.initializer(arg);
  }

private:
  static type_store &types() {
    static type_store TYPES;
    return TYPES;
  }
};

} // namespace yadi

#endif // YADI_FACTORY_HPP__
