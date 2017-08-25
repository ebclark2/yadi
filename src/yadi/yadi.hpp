#ifndef YADI_FACTORY_HPP__
#define YADI_FACTORY_HPP__

#include <yaml-cpp/yaml.h>

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace yadi {

template <typename T>
using bare_t = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

/**
 * @brief Factory traits that can be changed for base_t.
 * @tparam base_t
 */
template <typename base_t>
struct factory_traits {
    using ptr_type = std::unique_ptr<base_t>;  /// The type of pointer to return from create.
    static const bool direct_from_yaml = false;
};

/**
 * @brief The type of pointer the base_t factory creates.
 */
template <typename base_t>
using ptr_type_t = typename factory_traits<base_t>::ptr_type;

/**
 * @brief Determine is factory returns by value
 * @tparam base_t
 */
template <typename base_t>
struct is_by_value {
    static const bool value = std::is_same<base_t, ptr_type_t<base_t>>::value;
};

/**
 * A YAML based factory.
 * @tparam base_t
 */
template <typename base_t>
class factory {
   public:
    using base_type = base_t;
    using initializer_type = std::function<ptr_type_t<base_type>(YAML::Node)>;
    using ptr_type = ptr_type_t<base_type>;

    struct type_info {
        initializer_type initializer;
        std::string help;
    };

    using type_store = std::map<std::string, type_info>;

   public:
    /**
     * @brief Registers initializer to type.  When create is called with type this initializer will
     * be called. Overwrites initializer if already registered (will change).
     * @param type
     * @param initializer
     */
    static void register_type(std::string type, initializer_type initializer);

    /**
     * @brief Calls the initializer associated with type passing the given YAML config.
     * @param type
     * @param config
     * @return The result of the registered initializer
     * @throws std::runtime_error if no initializer is registered for type
     */
    static ptr_type create(std::string const& type, YAML::Node const& config = {});

    /**
     * @brief A copy of the stored types and their registered initializers and help.
     * @return
     */
    static type_store types();

   private:
    static type_store& mut_types();
};

template <typename base_t>
using initializer_type_t = typename factory<base_t>::initializer_type;

/**
 * @brief Same as factory<base_t>::create(type, config)
 * @tparam base_t
 * @param type
 * @param config
 * @return
 */
template <typename base_t>
ptr_type_t<base_t> create(std::string const& type, YAML::Node const& config = {});

/**
 * @brief Pulls type and config from YAML.  This function is especially usefil when loading
 * nested types from YAML configuration.  If factory_config is a scalar string it will be used
 * as type.  If factory_config is a map then "type" and "config" keys will be pulled from it and
 * used as such.
 * @param factory_config
 * @return
 */
template <typename base_t>
ptr_type_t<base_t> from_yaml(YAML::Node const& factory_config);

/**
 * @brief Populate output iterator from sequence of factory configs (anything from_yaml accepts).
 * @tparam base_t
 * @tparam output_iterator
 * @param factory_configs
 * @param out
 */
template <typename base_t, typename output_iterator>
void from_yamls(YAML::Node const& factory_configs, output_iterator out);

/**
 * If lest is same as right then provide type_type as type.
 * @tparam left
 * @tparam right
 * @tparam type_type
 */
template <typename left, typename right, typename type_type,
          typename = typename std::enable_if<std::is_same<left, right>::value>::type>
struct if_same_then {
    using type = type_type;
};

template <typename left, typename right, typename type_type>
using is_same_then_t = typename if_same_then<left, right, type_type>::type;

/**
 * @brief Helper to derive_base_type specialization by value.  Adds check to verify factory is actually by value.
 * @tparam T
 */
template <typename T, typename = typename std::enable_if<is_by_value<T>::value>::type>
struct derive_base_type_by_value {
    using base_type = T;
};

/**
 * For type T, provide the factory base type that creates T. For example, for T std::unique_ptr<int>, int would
 * be the base type.
 * @tparam T
 */
template <typename T>
struct derive_base_type {
    using base_type = typename derive_base_type_by_value<T>::base_type;
};

template <typename T>
using derive_base_type_t = typename derive_base_type<T>::base_type;

/**
 * @brief Populate out from factory config.  The factory type is derived from ptr_type.
 * @tparam ptr_type
 * @param out
 * @param factory_config
 */
template <typename ptr_type>
void parse(ptr_type& out, YAML::Node const& factory_config);

/**
 * @brief Returns config.as<T>().  Signature matches factory initializer.
 * @tparam T
 * @param config
 * @return
 */
template <typename T>
T yaml_as(YAML::Node const& config);

/**
 * @brief Constructs impl_t via a constructor that accepts YAML and returns as pointer to base_t,
 * ptr_type_t<base_t>.
 * @tparam base_t Use to determine pointer type.
 * @tparam impl_t Type to construct
 * @param config Argument to constructor.
 * @return The constructed type as ptr_type_t<base_t>
 */
template <typename base_t, typename impl_t>
ptr_type_t<base_t> yaml_init(YAML::Node const& config);

// TODO complete function
// TODO change argument names
/**
 * If both types are maps then they are merged.  If right is not defined then left is used.
 * Otherwise, error.
 * @param left
 * @return
 */
YAML::Node merge_yaml(YAML::Node const& left, YAML::Node const& /* right */);

/**
 * @brief Equivalent to factory<baes_t>::register_type(type, initializer)
 * @tparam base_t
 * @param type
 * @param initializer
 */
template <typename base_t>
void register_type(std::string type, initializer_type_t<base_t> initializer);

/**
 * @brief Registers type using yaml_init function as initializer.
 * @tparam base_t The factory type to use
 * @tparam impl_t The type to create
 * @param type The name to register the type under
 */
template <typename base_t, typename impl_t>
void register_type(std::string type);

/**
 * @brief Registers type to initializer that will construct impl_t using default constructor.
 * @tparam base_t
 * @tparam impl_t
 * @param type
 */
template <typename base_t, typename impl_t>
void register_type_no_arg(std::string type);

/**
 * @brief Registers alias to type and config pair.  When create is called for alias the passed in
 * and registered configs are merged and the initializer registered to type is called with the
 * result.
 * @tparam base_t
 * @param alias
 * @param type
 * @param config
 */
template <typename base_t>
void register_alias(std::string alias, std::string type, YAML::Node config);

/**
 * @brief Loads aliases from a YAML file.  The file should be a map of the format...
 * alias:
 *   type: actualType
 *   config: ...
 *
 * For each entry register_alias() is called.
 * @tparam base_t
 * @param aliases
 */
template <typename base_t>
void register_aliases(YAML::Node aliases);

/**
 * @brief Creates factory initializer that expects a YAML sequence.  The elements of the sequence will be
 * passed as a YAML factory config to the factory of the argument type.  The results will be passed in to function
 * func.
 * @tparam base_t
 * @tparam F
 * @param func
 * @return
 */
template <typename base_t, typename F>
::yadi::initializer_type_t<base_t> make_initializer(F func);

/**
 * @brief Expects a YAML map.  The fields are pulled from the map and their values are used to create a sequence
 * in the order the fields are provided.  Once the sequence is created it's treated the behavior is the same as
 * make_initializer(F).
 * @tparam base_t
 * @tparam F
 * @param func
 * @param fields
 * @return
 */
template <typename base_t, typename F>
::yadi::initializer_type_t<base_t> make_initializer(F func, std::vector<std::string> fields);

#define YADI_INIT_BEGIN_N(NAME)           \
    namespace {                           \
    struct static_initialization_##NAME { \
        static_initialization_##NAME() {
#define YADI_INIT_END_N(NAME)                                      \
    }                                                              \
    }                                                              \
    ;                                                              \
    static_initialization_##NAME static_initialization_##NAME##__; \
    }

#define YADI_INIT_BEGIN YADI_INIT_BEGIN_N(ANON)
#define YADI_INIT_END YADI_INIT_END_N(ANON)

/// Expose types yaml supports directly
#define YADI_YAML_TYPE_BY_VALUE(TYPE, INIT_NAME)     \
    template <>                                      \
    struct factory_traits<TYPE> {                    \
        using ptr_type = TYPE;                       \
        static const bool direct_from_yaml = true;   \
    };                                               \
                                                     \
    YADI_INIT_BEGIN_N(INIT_NAME)                     \
    ::yadi::register_type<TYPE>("", &yaml_as<TYPE>); \
    YADI_INIT_END_N(INIT_NAME)

#ifndef YADI_NO_STD_STRING
YADI_YAML_TYPE_BY_VALUE(std::string, string)
#endif
#ifndef YADI_NO_STD_INT
YADI_YAML_TYPE_BY_VALUE(int, int)
#endif
#ifndef YADI_NO_STD_DOUBLE
YADI_YAML_TYPE_BY_VALUE(double, double)
#endif

// ################# IMPL ################################
template <typename base_t>
ptr_type_t<base_t> create(std::string const& type, YAML::Node const& config) {
    return factory<base_t>::create(type, config);
}

template <typename base_t>
void factory<base_t>::register_type(std::string type, initializer_type initializer) {
    mut_types()[type].initializer = initializer;
}

template <typename base_t>
typename factory<base_t>::ptr_type factory<base_t>::create(std::string const& type, YAML::Node const& config) {
    typename type_store::const_iterator type_iter = mut_types().find(type);
    if (type_iter == mut_types().end()) {
        throw std::runtime_error(type + " not found");
    }

    return type_iter->second.initializer(config);
}

template <typename base_t>
typename factory<base_t>::type_store factory<base_t>::types() {
    return mut_types();
}

template <typename base_t>
typename factory<base_t>::type_store& factory<base_t>::mut_types() {
    static type_store TYPES;
    return TYPES;
}

template <typename base_t>
ptr_type_t<base_t> from_yaml(YAML::Node const& factory_config) {
    if (factory_traits<base_t>::direct_from_yaml) {
        return factory<base_t>::create("", factory_config);
    }

    if (!factory_config.IsDefined()) {
        throw std::runtime_error("Factory config not defined");
    }

    if (factory_config.IsScalar()) {
        std::string type = factory_config.as<std::string>("");
        if (type.empty()) {
            throw std::runtime_error("Factory config scalar not valid");
        }

        return factory<base_t>::create(type);
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
        return factory<base_t>::create(type, configNode);
    }

    throw std::runtime_error("Factory config not valid, YAML must be scalar string or map");
}

template <typename base_t, typename output_iterator>
void from_yamls(YAML::Node const& factory_configs, output_iterator out) {
    if (!factory_configs.IsDefined()) {
        throw std::runtime_error("From YAML factory configs not defined");
    }
    // If it's not a sequence then parse single
    if (!factory_configs.IsSequence()) {
        *out = from_yaml<base_t>(factory_configs);
        ++out;
        return;
    }

    // A sequence!
    for (YAML::Node const& entry : factory_configs) {
        *out = from_yaml<base_t>(entry);
        ++out;
    }
}

template <typename ptr_type>
void parse(ptr_type& out, YAML::Node const& factory_config) {
    out = from_yaml<typename derive_base_type<ptr_type>::base_type>(factory_config);
}

template <typename T>
T yaml_as(YAML::Node const& config) {
    // TODO Improved error message
    return config.as<T>();
}

template <typename base_t, typename impl_t,
          typename = typename std::enable_if<is_by_value<base_t>::value && std::is_same<base_t, impl_t>::value>::type>
struct yaml_init_value {
    static ptr_type_t<base_t> init(YAML::Node const& config) {
        base_t ret(config);
        return ret;
    }
};

template <typename base_t, typename impl_t, bool by_value = is_by_value<base_t>::value>
struct yaml_init_helper {};

template <typename base_t, typename impl_t>
struct yaml_init_helper<base_t, impl_t, false> {
    static ptr_type_t<base_t> init(YAML::Node const& config) {
        ptr_type_t<base_t> ret(new impl_t(config));
        return ret;
    }
};

template <typename base_t, typename impl_t>
struct yaml_init_helper<base_t, impl_t, true> {
    static ptr_type_t<base_t> init(YAML::Node const& config) { return yaml_init_value<base_t, impl_t>::init(config); }
};

template <typename base_t, typename impl_t>
ptr_type_t<base_t> yaml_init(YAML::Node const& config) {
    return yaml_init_helper<base_t, impl_t>::init(config);
};

template <typename base_t, typename impl_t,
          typename = typename std::enable_if<is_by_value<base_t>::value && std::is_same<base_t, impl_t>::value>::type>
struct no_arg_init_value {
    static ptr_type_t<base_t> init() {
        base_t ret;
        return ret;
    }
};

template <typename base_t, typename impl_t, bool by_value = is_by_value<base_t>::value>
struct no_arg_init_helper {};

template <typename base_t, typename impl_t>
struct no_arg_init_helper<base_t, impl_t, false> {
    static ptr_type_t<base_t> init() {
        ptr_type_t<base_t> ret(new impl_t);
        return ret;
    }
};

template <typename base_t, typename impl_t>
struct no_arg_init_helper<base_t, impl_t, true> {
    static ptr_type_t<base_t> init() { return no_arg_init_value<base_t, impl_t>::init(); }
};

template <typename base_t, typename impl_t>
ptr_type_t<base_t> no_arg_init(YAML::Node const&) {
    return no_arg_init_helper<base_t, impl_t>::init();
};

inline YAML::Node merge_yaml(YAML::Node const& left, YAML::Node const& /* right */) { return left; }

template <typename base_t>
void register_type(std::string type, initializer_type_t<base_t> initializer) {
    factory<base_t>::register_type(type, initializer);
};

template <typename base_t, typename impl_t>
void register_type(std::string type) {
    register_type<base_t>(type, &yaml_init<base_t, impl_t>);
};

template <typename base_t, typename impl_t>
void register_type_no_arg(std::string type) {
    register_type<base_t>(type, &no_arg_init<base_t, impl_t>);
}

template <typename base_t>
void register_alias(std::string alias, std::string type, YAML::Node config) {
    register_type<base_t>(alias, [type, config](YAML::Node const& passedConfig) {
        YAML::Node mergedConfig = merge_yaml(config, passedConfig);
        return factory<base_t>::create(type, mergedConfig);
    });
}

template <typename base_t>
void register_aliases(YAML::Node aliases) {
    // TODO error handling
    std::map<std::string, YAML::Node> aliasesMap = aliases.as<std::map<std::string, YAML::Node>>();
    for (auto const& entry : aliasesMap) {
        std::string type = entry.second["type"].as<std::string>();
        YAML::Node config = entry.second["config"];
        register_alias<base_t>(entry.first, type, config);
    }
}

template <typename tuple_t, size_t index = std::tuple_size<tuple_t>::value - 1>
struct yaml_to_tuple {
    static void to_tuple(tuple_t& out, YAML::Node const& yaml) {
        using element_type = bare_t<std::tuple_element_t<std::tuple_size<tuple_t>::value - 1 - index, tuple_t>>;
        std::get<std::tuple_size<tuple_t>::value - 1 - index>(out) =
            ::yadi::from_yaml<derive_base_type_t<element_type>>(yaml[std::tuple_size<tuple_t>::value - 1 - index]);
        yaml_to_tuple<tuple_t, index - 1>::to_tuple(out, yaml);
    }
};

template <typename tuple_t>
struct yaml_to_tuple<tuple_t, 0> {
    static void to_tuple(tuple_t& out, YAML::Node const& yaml) {
        using element_type = bare_t<std::tuple_element_t<std::tuple_size<tuple_t>::value - 1, tuple_t>>;
        std::get<std::tuple_size<tuple_t>::value - 1>(out) =
            ::yadi::from_yaml<derive_base_type_t<element_type>>(yaml[std::tuple_size<tuple_t>::value - 1]);
    }
};

template <typename T>
struct function_traits;

template <typename R, typename... Args>
struct function_traits<R (*)(Args...)> {
    using result_type = R;
    using params_type = std::tuple<bare_t<Args>...>;
    using function_type = std::function<R(Args...)>;
    using index_sequence = std::index_sequence_for<Args...>;
};

template <typename R, typename... Args>
struct function_traits<std::function<R(Args...)>> {
    using result_type = R;
    using params_type = std::tuple<bare_t<Args>...>;
    using function_type = std::function<R(Args...)>;
    using index_sequence = std::index_sequence_for<Args...>;
};

template <typename T>
using function_traits_params_type = typename function_traits<T>::params_type;

template <typename T>
using function_traits_result_type = typename function_traits<T>::result_type;

template <typename T>
using function_traits_function_type = typename function_traits<T>::function_type;

template <typename T>
using function_traits_index_sequence = typename function_traits<T>::index_sequence;

template <typename T>
struct function_call_via_yaml {
    using result_type = function_traits_result_type<T>;
    using params_type = function_traits_params_type<T>;
    using function_type = function_traits_function_type<T>;
    using index_sequence = function_traits_index_sequence<T>;

    static result_type call(function_type func, YAML::Node const& yaml);

   private:
    function_call_via_yaml(function_type func, params_type* params) : func(std::move(func)), params(params) {}

    template <std::size_t... I>
    result_type call_func(std::index_sequence<I...>) {
        return func(std::get<I>(*params)...);
    }

    result_type delayed_dispatch() { return call_func(index_sequence{}); }

    function_type func;
    params_type* params;
};

template <typename T>
typename function_call_via_yaml<T>::result_type function_call_via_yaml<T>::call(function_type func,
                                                                                YAML::Node const& yaml) {
    // Use std::apply in c++17
    params_type params;
    yaml_to_tuple<params_type>::to_tuple(params, yaml);
    function_call_via_yaml<T> f{func, &params};
    return f.delayed_dispatch();
}

template <typename F>
function_traits_result_type<F> call_from_yaml(F const& func, YAML::Node const& yaml) {
    return function_call_via_yaml<F>::call(func, yaml);
}

template <typename T>
struct derive_base_type<std::shared_ptr<T>> {
    using base_type = is_same_then_t<ptr_type_t<T>, std::shared_ptr<T>, T>;
};

template <typename T>
struct derive_base_type<std::unique_ptr<T>> {
    using base_type = is_same_then_t<ptr_type_t<T>, std::unique_ptr<T>, T>;
};

template <typename T>
struct derive_base_type<T*> {
    using base_type = is_same_then_t<ptr_type_t<T>, T*, T>;
};

template <typename base_t, typename F>
::yadi::initializer_type_t<base_t> make_initializer(F func) {
    // TODO Error checking for yaml sequence type
    return [func](YAML::Node const& yaml) { return call_from_yaml(func, yaml); };
};

template <typename base_t, typename F>
::yadi::initializer_type_t<base_t> make_initializer(F func, std::vector<std::string> fields) {
    return [func, fields](YAML::Node const& yaml) {
        // Convert yaml map to sequence via ordered field list
        // TODO error checking for yaml map type
        YAML::Node sequence;
        for (std::string const& field : fields) {
            // TODO error checking for field
            sequence.push_back(yaml[field]);
        }
        return call_from_yaml(func, sequence);
    };
};

}  // namespace yadi

#endif  // YADI_FACTORY_HPP__
