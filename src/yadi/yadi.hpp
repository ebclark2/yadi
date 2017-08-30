#ifndef YADI_FACTORY_HPP__
#define YADI_FACTORY_HPP__

#include "details/create_utils.hpp"
#include "details/demangle.hpp"
#include "details/factory.hpp"
#include "details/factory_specializations.hpp"
#include "details/help.hpp"
#include "details/initializers.hpp"

#include <yaml-cpp/yaml.h>

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace yadi {

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

template <typename BT>
static void register_factory(std::string name);

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

#define YADI_INIT_BEGIN YADI_INIT_BEGIN_N(ANON)
#define YADI_INIT_END YADI_INIT_END_N(ANON)

/// Expose types yaml supports directly
#define YADI_YAML_TYPE_BY_VALUE_DECL(TYPE, INIT_NAME) \
    template <>                                       \
    struct factory_traits<TYPE> {                     \
        using ptr_type = TYPE;                        \
        static const bool direct_from_yaml = true;    \
    };

#define YADI_YAML_TYPE_BY_VALUE_DEFN(TYPE, INIT_NAME)                                            \
    YADI_INIT_BEGIN_N(INIT_NAME)                                                                 \
    ::yadi::register_type<TYPE>(::yadi::type_by_value_key(), ::yadi::yaml_as_with_help<TYPE>()); \
    ::yadi::register_factory<TYPE>(#TYPE);                                                       \
    YADI_INIT_END_N(INIT_NAME)

#ifndef YADI_NO_STD_STRING
YADI_YAML_TYPE_BY_VALUE_DECL(std::string, string)
#endif
#ifndef YADI_NO_STD_INT
YADI_YAML_TYPE_BY_VALUE_DECL(int, int)
#endif
#ifndef YADI_NO_STD_DOUBLE
YADI_YAML_TYPE_BY_VALUE_DECL(double, double)
#endif

// ################# IMPL ################################
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

template <typename BT>
static void register_factory(std::string name) {
    yadi_help::register_factory(name, factory<BT>::types());
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
        arg_types = demangle_type<element_type>();
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
