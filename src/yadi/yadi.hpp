#ifndef YADI_FACTORY_HPP__
#define YADI_FACTORY_HPP__

#include <yaml-cpp/yaml.h>
#include <boost/core/demangle.hpp>

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace yadi {

extern std::string const TYPE_BY_VALUE;

template <typename T>
using bare_t = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

/**
 * @brief Factory traits that can be changed for BT.
 * @tparam BT
 */
template <typename BT>
struct factory_traits {
    using ptr_type = std::unique_ptr<BT>;  /// The type of pointer to return from create.
    static const bool direct_from_yaml = false;
};

/**
 * @brief The type of pointer the BT factory creates.
 */
template <typename BT>
using ptr_type_t = typename factory_traits<BT>::ptr_type;

/**
 * @brief Determine is factory returns by value
 * @tparam BT
 */
template <typename BT>
struct is_by_value {
    static const bool value = std::is_same<BT, ptr_type_t<BT>>::value;
};

// TODO YADI_DECL and YADI_DEFN macros with bit to add factory type to something for help retrieval
template <typename BT>
class factory {
   public:
    using base_type = BT;
    using initializer_type = std::function<ptr_type_t<base_type>(YAML::Node)>;
    using ptr_type = ptr_type_t<base_type>;

    struct yadi_info {
        initializer_type initializer;
        std::string help;
    };

    using type_store = std::map<std::string, yadi_info>;

    /**
     * @brief Registers initializer to type.  When create is called with type this initializer will
     * be called. Overwrites initializer if already registered (will change).
     * @param type
     * @param initializer
     */
    static void register_type(std::string type, yadi_info yadis);

    /**
     * @brief Calls the initializer associated with type passing the given YAML config.
     * @param type
     * @param config
     * @return The result of the registered initializer
     * @throws std::runtime_error if no initializer is registered for type
     */
    static ptr_type create(std::string const& type, YAML::Node const& config = {});

    /**
     * @brief Stored types and their registered initializers and help.
     * @return
     */
    static type_store const& types();

   private:
    static type_store& mut_types();
};

struct yadi_help_fetcher {
    struct concept {
        virtual std::string getHelp(std::string const& type) const = 0;
        virtual std::vector<std::string> getTypes() const = 0;
        virtual std::unique_ptr<concept> clone() const = 0;
    };

    template <typename T>
    struct model : public concept {
        model(T const& types) : types(types) {}

        std::string getHelp(std::string const& type) const override {
            auto types_iter = this->types.find(type);
            if (types_iter == this->types.end()) {
                throw std::runtime_error("Type \"" + type + "\" not found");
            }

            return types_iter->second.help;
        }

        std::vector<std::string> getTypes() const override {
            std::vector<std::string> ret;
            for (auto const& entry : this->types) {
                ret.push_back(entry.first);
            }

            return ret;
        }

        std::unique_ptr<concept> clone() const override {
            std::unique_ptr<concept> copy(new model(this->types));
            return copy;
        }

        T const& types;
    };

    yadi_help_fetcher();

    yadi_help_fetcher(yadi_help_fetcher const& other);

    template <typename Y>
    yadi_help_fetcher(Y const& types) : impl(new model<Y>(types)) {}

    yadi_help_fetcher& operator=(yadi_help_fetcher const& other);

    inline std::string getHelp(std::string const& type) const { return this->impl->getHelp(type); }
    inline std::vector<std::string> getTypes() const { return this->impl->getTypes(); }

   private:
    std::unique_ptr<concept> impl;
};

struct yadi_help {
    using help_store = std::map<std::string, yadi_help_fetcher>;

    template <typename BT>
    static void register_factory(std::string name) {
        mut_helps()[name] = factory<BT>::types();
    }

    static help_store const& helps();

   private:
    static help_store& mut_helps();
};

template <typename BT>
using initializer_type_t = typename factory<BT>::initializer_type;

// TODO Comment here
template <typename BT>
using yadi_info_t = typename factory<BT>::yadi_info;

/**
 * @brief Same as factory<BT>::create(type, config)
 * @tparam BT
 * @param type
 * @param config
 * @return
 */
template <typename BT>
ptr_type_t<BT> create(std::string const& type, YAML::Node const& config = {});

/**
 * @brief Pulls type and config from YAML.  This function is especially usefil when loading
 * nested types from YAML configuration.  If factory_config is a scalar string it will be used
 * as type.  If factory_config is a map then "type" and "config" keys will be pulled from it and
 * used as such.
 * @param factory_config
 * @return
 */
template <typename BT>
ptr_type_t<BT> from_yaml(YAML::Node const& factory_config);

/**
 * @brief Populate output iterator from sequence of factory configs (anything from_yaml accepts).
 * @tparam BT base type
 * @tparam OI Output iterator
 * @param factory_configs
 * @param out
 */
template <typename BT, typename OI>
void from_yamls(YAML::Node const& factory_configs, OI out);

/**
 * If lest is same as right then provide type_type as type.
 * @tparam L left
 * @tparam R right
 * @tparam RT result type
 */
template <typename L, typename R, typename RT, typename = typename std::enable_if<std::is_same<L, R>::value>::type>
struct if_same_then {
    using type = RT;
};

template <typename L, typename R, typename RT>
using is_same_then_t = typename if_same_then<L, R, RT>::type;

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
 * @tparam PT pointer type
 * @param out
 * @param factory_config
 */
template <typename PT>
void parse(PT& out, YAML::Node const& factory_config);

/**
 * @brief Returns config.as<T>().  Signature matches factory initializer.
 * @tparam T
 * @param config
 * @return
 */
template <typename T>
T yaml_as(YAML::Node const& config);

// TODO comment
template <typename T>
yadi_info_t<T> yaml_as_with_help();

/**
 * @brief Constructs IT via a constructor that accepts YAML and returns as pointer to BT,
 * ptr_type_t<BT>.
 * @tparam BT Use to determine pointer type.
 * @tparam IT Implementation type.  Type to construct
 * @param config Argument to constructor.
 * @return The constructed type as ptr_type_t<BT>
 */
template <typename BT, typename IT>
ptr_type_t<BT> yaml_init(YAML::Node const& config);

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
 * @brief Equibalent to factory<BT>::register_type(type, yadis)
 * @tparam BT
 * @param type
 * @param initializer
 */
template <typename BT>
void register_type(std::string type, yadi_info_t<BT> yadis);

// TODO Update comment
/**
 * @brief
 * @tparam BT
 * @param type
 * @param initializer
 */
template <typename BT>
void register_type(std::string type, initializer_type_t<BT> initializer);

/**
 * @brief Registers type using yaml_init function as initializer.
 * @tparam BT The factory type to use
 * @tparam IT The type to create
 * @param type The name to register the type under
 */
template <typename BT, typename IT>
void register_type(std::string type);

/**
 * @brief Registers type to initializer that will construct IT using default constructor.
 * @tparam BT
 * @tparam IT
 * @param type
 */
template <typename BT, typename IT>
void register_type_no_arg(std::string type);

/**
 * @brief Registers alias to type and config pair.  When create is called for alias the passed in
 * and registered configs are merged and the initializer registered to type is called with the
 * result.
 * @tparam BT
 * @param alias
 * @param type
 * @param config
 */
template <typename BT>
void register_alias(std::string alias, std::string type, YAML::Node config);

/**
 * @brief Loads aliases from a YAML file.  The file should be a map of the format...
 * alias:
 *   type: actualType
 *   config: ...
 *
 * For each entry register_alias() is called.
 * @tparam BT
 * @param aliases
 */
template <typename BT>
void register_aliases(YAML::Node aliases);

/**
 * @brief Creates factory initializer that expects a YAML sequence.  The elements of the sequence will be
 * passed as a YAML factory config to the factory of the argument type.  The results will be passed in to function
 * func.
 * @tparam BT
 * @tparam F
 * @param func
 * @return
 */
template <typename BT, typename F>
initializer_type_t<BT> make_initializer(F func);

/**
 * @brief Makes yadi info with generated help for F
 * @tparam BT base type
 * @tparam F Function type
 * @param func Function to find yaml to
 * @return Generated yadi info
 */
template <typename BT, typename F>
yadi_info_t<BT> make_initializer_with_help(F func);

/**
 * @brief Expects a YAML map.  The fields are pulled from the map and their values are used to create a sequence
 * in the order the fields are provided.  Once the sequence is created it's treated the behavior is the same as
 * make_initializer(F).
 * @tparam BT
 * @tparam F
 * @param func
 * @param fields
 * @return
 */
template <typename BT, typename F>
initializer_type_t<BT> make_initializer(F func, std::vector<std::string> fields);

// TODO Comment
template <typename BT, typename F>
yadi_info_t<BT> make_initializer_with_help(F func, std::vector<std::string> fields);

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

std::string demangle(const char* name);

template <typename T>
std::string demangle_type() {
    return demangle(typeid(T).name());
}

#define YADI_INIT_BEGIN YADI_INIT_BEGIN_N(ANON)
#define YADI_INIT_END YADI_INIT_END_N(ANON)

/// Expose types yaml supports directly
#define YADI_YAML_TYPE_BY_VALUE(TYPE, INIT_NAME)                           \
    template <>                                                            \
    struct factory_traits<TYPE> {                                          \
        using ptr_type = TYPE;                                             \
        static const bool direct_from_yaml = true;                         \
    };                                                                     \
                                                                           \
    YADI_INIT_BEGIN_N(INIT_NAME)                                           \
    ::yadi::register_type<TYPE>(TYPE_BY_VALUE, yaml_as_with_help<TYPE>()); \
    ::yadi::yadi_help::register_factory<TYPE>(#TYPE);                      \
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

    static ptr_type create(std::string const& /*type*/, YAML::Node const& config = {}) {
        std::vector<T> ret;
        from_yamls<T>(config, std::back_inserter(ret));
        return ret;
    }
};

template <typename BT>
ptr_type_t<BT> create(std::string const& type, YAML::Node const& config) {
    return factory<BT>::create(type, config);
}

template <typename BT>
void factory<BT>::register_type(std::string type, yadi_info yadis) {
    mut_types()[type] = yadis;
}

template <typename BT>
typename factory<BT>::ptr_type factory<BT>::create(std::string const& type, YAML::Node const& config) {
    typename type_store::const_iterator type_iter = mut_types().find(type);
    if (type_iter == mut_types().end()) {
        throw std::runtime_error(type + " not found");
    }

    return type_iter->second.initializer(config);
}

template <typename BT>
typename factory<BT>::type_store const& factory<BT>::types() {
    return mut_types();
}

template <typename BT>
typename factory<BT>::type_store& factory<BT>::mut_types() {
    static type_store TYPES;
    return TYPES;
}

template <typename BT>
ptr_type_t<BT> from_yaml(YAML::Node const& factory_config) {
    if (factory_traits<BT>::direct_from_yaml) {
        return factory<BT>::create(TYPE_BY_VALUE, factory_config);
    }

    if (!factory_config.IsDefined()) {
        throw std::runtime_error("Factory config not defined");
    }

    if (factory_config.IsScalar()) {
        std::string type = factory_config.as<std::string>("");
        if (type.empty()) {
            throw std::runtime_error("Factory config scalar not valid");
        }

        return factory<BT>::create(type);
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
        return factory<BT>::create(type, configNode);
    }

    throw std::runtime_error("Factory config not valid, YAML must be scalar string or map");
}

template <typename BT, typename output_iterator>
void from_yamls(YAML::Node const& factory_configs, output_iterator out) {
    if (!factory_configs.IsDefined()) {
        throw std::runtime_error("From YAML factory configs not defined");
    }
    // If it's not a sequence then parse single
    if (!factory_configs.IsSequence()) {
        *out = from_yaml<BT>(factory_configs);
        ++out;
        return;
    }

    // A sequence!
    for (YAML::Node const& entry : factory_configs) {
        *out = from_yaml<BT>(entry);
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

template <typename T>
yadi_info_t<T> yaml_as_with_help() {
    // TODO Improved error message
    return {&yaml_as<T>, "Direct conversion using yaml.as<T>()"};
}

template <typename BT, typename IT,
          typename = typename std::enable_if<is_by_value<BT>::value && std::is_same<BT, IT>::value>::type>
struct yaml_init_value {
    static ptr_type_t<BT> init(YAML::Node const& config) {
        BT ret(config);
        return ret;
    }
};

template <typename BT, typename IT, bool by_value = is_by_value<BT>::value>
struct yaml_init_helper {};

template <typename BT, typename IT>
struct yaml_init_helper<BT, IT, false> {
    static ptr_type_t<BT> init(YAML::Node const& config) {
        ptr_type_t<BT> ret(new IT(config));
        return ret;
    }
};

template <typename BT, typename IT>
struct yaml_init_helper<BT, IT, true> {
    static ptr_type_t<BT> init(YAML::Node const& config) { return yaml_init_value<BT, IT>::init(config); }
};

template <typename BT, typename IT>
ptr_type_t<BT> yaml_init(YAML::Node const& config) {
    return yaml_init_helper<BT, IT>::init(config);
};

template <typename BT, typename IT,
          typename = typename std::enable_if<is_by_value<BT>::value && std::is_same<BT, IT>::value>::type>
struct no_arg_init_value {
    static ptr_type_t<BT> init() {
        BT ret;
        return ret;
    }
};

template <typename BT, typename IT, bool by_value = is_by_value<BT>::value>
struct no_arg_init_helper {};

template <typename BT, typename IT>
struct no_arg_init_helper<BT, IT, false> {
    static ptr_type_t<BT> init() {
        ptr_type_t<BT> ret(new IT);
        return ret;
    }
};

template <typename BT, typename IT>
struct no_arg_init_helper<BT, IT, true> {
    static ptr_type_t<BT> init() { return no_arg_init_value<BT, IT>::init(); }
};

template <typename BT, typename IT>
ptr_type_t<BT> no_arg_init(YAML::Node const&) {
    return no_arg_init_helper<BT, IT>::init();
};

inline YAML::Node merge_yaml(YAML::Node const& left, YAML::Node const& /* right */) { return left; }

template <typename BT>
void register_type(std::string type, yadi_info_t<BT> yadis) {
    factory<BT>::register_type(type, yadis);
};

template <typename BT>
void register_type(std::string type, initializer_type_t<BT> initializer) {
    register_type<BT>(type, {initializer, "No help provided"});
};

template <typename BT, typename IT>
void register_type(std::string type) {
    register_type<BT>(type, {&yaml_init<BT, IT>, "Expects config.  Maybe add some help..."});
};

template <typename BT, typename IT>
void register_type_no_arg(std::string type) {
    register_type<BT>(type, {&no_arg_init<BT, IT>, "No config"});
}

template <typename BT>
void register_alias(std::string alias, std::string type, YAML::Node config) {
    register_type<BT>(alias, [type, config](YAML::Node const& passedConfig) {
        YAML::Node mergedConfig = merge_yaml(config, passedConfig);
        return factory<BT>::create(type, mergedConfig);
    });
}

template <typename BT>
void register_aliases(YAML::Node aliases) {
    // TODO error handling
    std::map<std::string, YAML::Node> aliasesMap = aliases.as<std::map<std::string, YAML::Node>>();
    for (auto const& entry : aliasesMap) {
        std::string type = entry.second["type"].as<std::string>();
        YAML::Node config = entry.second["config"];
        register_alias<BT>(entry.first, type, config);
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

    template <typename arg_type_out>
    static void to_arg_types(arg_type_out arg_types) {
        using element_type = bare_t<std::tuple_element_t<std::tuple_size<tuple_t>::value - 1 - index, tuple_t>>;
        // TODO demangle
        arg_types = boost::core::demangled_name(typeid(element_type));
        arg_types++;
        yaml_to_tuple<tuple_t, index - 1>::to_arg_types(arg_types);
    }
};

// TODO Factory name registery.  Use this before demangled name, maybe only
template <typename tuple_t>
struct yaml_to_tuple<tuple_t, 0> {
    static void to_tuple(tuple_t& out, YAML::Node const& yaml) {
        using element_type = bare_t<std::tuple_element_t<std::tuple_size<tuple_t>::value - 1, tuple_t>>;
        std::get<std::tuple_size<tuple_t>::value - 1>(out) =
            ::yadi::from_yaml<derive_base_type_t<element_type>>(yaml[std::tuple_size<tuple_t>::value - 1]);
    }

    template <typename arg_type_out>
    static void to_arg_types(arg_type_out arg_types) {
        using element_type = bare_t<std::tuple_element_t<std::tuple_size<tuple_t>::value - 1, tuple_t>>;
        // TODO demangle
        arg_types = boost::core::demangled_name(typeid(element_type));  // demangle_type<element_type>();
        arg_types++;
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

template <typename BT, typename F>
initializer_type_t<BT> make_initializer(F func) {
    // TODO Error checking for yaml sequence type
    return [func](YAML::Node const& yaml) { return call_from_yaml(func, yaml); };
};

template <typename BT, typename F>
yadi_info_t<BT> make_initializer_with_help(F func) {
    std::vector<std::string> field_types;
    yaml_to_tuple<function_traits_params_type<F>>::to_arg_types(std::back_inserter(field_types));
    std::string help = "Expects yaml sequence with types:";
    for (size_t i = 0; i < field_types.size(); ++i) {
        std::string const& field_type = field_types[i];
        help += "\n\t\t - ";
        help += field_type;
    }
    return {make_initializer<BT>(func), help};
}

template <typename BT, typename F>
initializer_type_t<BT> make_initializer(F func, std::vector<std::string> fields) {
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

template <typename BT, typename F>
yadi_info_t<BT> make_initializer_with_help(F func, std::vector<std::string> fields) {
    std::vector<std::string> field_types;
    yaml_to_tuple<function_traits_params_type<F>>::to_arg_types(std::back_inserter(field_types));
    if (field_types.size() != fields.size()) {
        throw std::runtime_error("Field count must match argument count");
    }
    std::string help = "Expects yaml map with fields:";
    for (size_t i = 0; i < fields.size(); ++i) {
        std::string const& field = fields[i];
        std::string const& field_type = field_types[i];
        help += "\n\t\t";
        help += field + "(" + field_type + ")";
    }
    return {make_initializer<BT>(func, fields), help};
}
}  // namespace yadi

#endif  // YADI_FACTORY_HPP__
