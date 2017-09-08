//
// Created by Ed Clark on 8/29/17.
//

#ifndef YADI_INITIALIZERS_HPP
#define YADI_INITIALIZERS_HPP

#include "create_specializations.hpp"
#include "create_utils.hpp"
#include "factory.hpp"
#include "help.hpp"
#include "type_utils.hpp"

/**
 * @namespace yadi
 * @brief YADI
 */
namespace yadi {

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

/**
 * @brief Call no argument constructor.
 * @tparam BT Base type.
 * @tparam IT Implementation type.
 * @return The pointer type of base type.
 */
template <typename BT, typename IT>
ptr_type_t<BT> no_arg_init(YAML::Node const&);

/**
 * @brief Construct IT via contructor with the given arguments.  Arguments are moved to constructor.
 * @tparam BT Base type.
 * @tparam IT Implementation type.
 * @tparam ARGS Argument types of constructor.
 * @param args Arguments for constructor.
 * @return ptr_type_t<BT> via constructor of IT.
 */
template <typename BT, typename IT, typename... ARGS>
ptr_type_t<BT> ctr_init(ARGS... args);

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
initializer_type_t<BT> make_sequence_initializer(F func);

/**
 * @brief Makes yadi info with generated help for F
 * @tparam BT base type
 * @tparam F Function type
 * @param func Function to find yaml to
 * @return Generated yadi info
 */
template <typename BT, typename F>
yadi_info_t<BT> make_sequence_initializer_with_help(F func, std::vector<std::string> helps = {});

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
initializer_type_t<BT> make_map_initializer(F func, std::vector<std::string> fields);

// TODO Comment
template <typename BT, typename F>
yadi_info_t<BT> make_map_initializer_with_help(F func, std::vector<std::string> fields);

template <typename BT, typename F>
yadi_info_t<BT> make_map_initializer_with_help(F func,
                                               std::vector<std::pair<std::string, std::string>> fields_with_help);

template <typename BT, typename F>
yadi_info_t<BT> make_map_initializer_with_help(F func, std::vector<std::string> fields,
                                               std::vector<std::string> fields_help);

// TODO thread safe
template <typename BT>
initializer_type_t<BT> make_caching_initializer(initializer_type_t<BT> const& initializing_initializer);

template <typename BT>
yadi_info_t<BT> make_caching_initializer(yadi_info_t<BT> yi);

// ################### IMPL ######################

namespace details {
template <typename BT, typename IT,
          typename = typename std::enable_if<meta::is_by_value<BT>::value && std::is_same<BT, IT>::value>::type>
struct yaml_init_value {
    static ptr_type_t<BT> init(YAML::Node const& config) {
        BT ret(config);
        return ret;
    }
};

template <typename BT, typename IT, bool by_value = meta::is_by_value<BT>::value>
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

template <typename BT, typename IT,
          typename = typename std::enable_if<meta::is_by_value<BT>::value && std::is_same<BT, IT>::value>::type>
struct no_arg_init_value {
    static ptr_type_t<BT> init() {
        BT ret;
        return ret;
    }
};

template <typename BT, typename IT, bool by_value = meta::is_by_value<BT>::value>
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

template <typename BT, typename IT, bool = meta::is_by_value<BT>::value>
struct ctr_init_helper;

template <typename BT, typename IT>
struct ctr_init_helper<BT, IT, true> {
    template <typename... ARGS>
    static ptr_type_t<BT> init(ARGS... args) {
        IT instance(std::move(args)...);
        return instance;
    }
};

template <typename BT, typename IT>
struct ctr_init_helper<BT, IT, false> {
    template <typename... ARGS>
    static ptr_type_t<BT> init(ARGS... args) {
        ptr_type_t<BT> instance(new IT(std::move(args)...));
        return instance;
    }
};

template <typename tuple_t, size_t index = std::tuple_size<tuple_t>::value - 1>
struct yaml_to_tuple {
    static void to_tuple(tuple_t& out, YAML::Node const& yaml) {
        using element_type = meta::bare_t<std::tuple_element_t<std::tuple_size<tuple_t>::value - 1 - index, tuple_t>>;
        std::get<std::tuple_size<tuple_t>::value - 1 - index>(out) =
            ::yadi::from_yaml<element_type>(yaml[std::tuple_size<tuple_t>::value - 1 - index]);
        yaml_to_tuple<tuple_t, index - 1>::to_tuple(out, yaml);
    }

    template <typename arg_type_out>
    static void to_arg_types(arg_type_out arg_types) {
        using element_type = meta::bare_t<std::tuple_element_t<std::tuple_size<tuple_t>::value - 1 - index, tuple_t>>;
        arg_types = adapter<element_type, element_type>::get_name();
        arg_types++;
        yaml_to_tuple<tuple_t, index - 1>::to_arg_types(arg_types);
    }
};

template <typename tuple_t>
struct yaml_to_tuple<tuple_t, 0> {
    static void to_tuple(tuple_t& out, YAML::Node const& yaml) {
        using element_type = meta::bare_t<std::tuple_element_t<std::tuple_size<tuple_t>::value - 1, tuple_t>>;
        std::get<std::tuple_size<tuple_t>::value - 1>(out) =
            ::yadi::from_yaml<element_type>(yaml[std::tuple_size<tuple_t>::value - 1]);
    }

    template <typename arg_type_out>
    static void to_arg_types(arg_type_out arg_types) {
        using element_type = meta::bare_t<std::tuple_element_t<std::tuple_size<tuple_t>::value - 1, tuple_t>>;
        arg_types = adapter<element_type, element_type>::get_name();
        arg_types++;
    }
};

template <typename T>
struct function_traits;

template <typename R, typename... Args>
struct function_traits<R (*)(Args...)> {
    using result_type = R;
    using params_type = std::tuple<meta::bare_t<Args>...>;
    using function_type = std::function<R(Args...)>;
    using index_sequence = std::index_sequence_for<Args...>;
};

template <typename R, typename... Args>
struct function_traits<std::function<R(Args...)>> {
    using result_type = R;
    using params_type = std::tuple<meta::bare_t<Args>...>;
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
        //        return func(static_cast<typename std::tuple_element<I, params_type>::type
        //        &&>(std::get<I>(*params))...);
        return func(std::move(std::get<I>(*params))...);
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
}  // namespace details

template <typename T>
T yaml_as(YAML::Node const& config) {
    // TODO Improved error message
    return config.as<T>();
}

template <typename T>
yadi_info_t<T> yaml_as_with_help() {
    // TODO Improved error message
    return {&yaml_as<T>, "Direct conversion using yaml.as<" + adapter<T>::get_name() + ">()"};
}

template <typename BT, typename IT>
ptr_type_t<BT> yaml_init(YAML::Node const& config) {
    return details::yaml_init_helper<BT, IT>::init(config);
}

template <typename BT, typename IT>
ptr_type_t<BT> no_arg_init(YAML::Node const&) {
    return details::no_arg_init_helper<BT, IT>::init();
}

template <typename BT, typename IT, typename... ARGS>
ptr_type_t<BT> ctr_init(ARGS... args) {
    return details::ctr_init_helper<BT, IT>::init(std::move(args)...);
}

template <typename BT, typename F>
initializer_type_t<BT> make_sequence_initializer(F func) {
    // TODO Error checking for yaml sequence type
    return [func](YAML::Node const& yaml) { return details::call_from_yaml(func, yaml); };
}

template <typename BT, typename F>
yadi_info_t<BT> make_sequence_initializer_with_help(F func, std::vector<std::string> helps) {
    std::vector<std::string> field_types;
    details::yaml_to_tuple<details::function_traits_params_type<F>>::to_arg_types(std::back_inserter(field_types));
    std::string help = "Expects yaml sequence with types:";
    for (size_t i = 0; i < field_types.size(); ++i) {
        std::string const& field_type = field_types[i];
        help += "\n\t\t - ";
        help += field_type;
        if (helps.size() > i) {
            help += ", " + helps[i];
        }
    }
    return {make_sequence_initializer<BT>(func), help};
}

template <typename BT, typename F>
initializer_type_t<BT> make_map_initializer(F func, std::vector<std::string> fields) {
    return [func, fields](YAML::Node const& yaml) {
        // Convert yaml map to sequence via ordered field list
        // TODO error checking for yaml map type
        YAML::Node sequence;
        for (std::string const& field : fields) {
            // TODO error checking for field
            sequence.push_back(yaml[field]);
        }
        return details::call_from_yaml(func, sequence);
    };
}

template <typename BT, typename F>
yadi_info_t<BT> make_map_initializer_with_help(F func, std::vector<std::string> fields) {
    return make_map_initializer_with_help<BT>(func, fields, {});
}

template <typename BT, typename F>
yadi_info_t<BT> make_map_initializer_with_help(F func,
                                               std::vector<std::pair<std::string, std::string>> fields_with_help) {
    std::vector<std::string> fields;
    std::vector<std::string> fields_help;
    for (auto const& field_with_help : fields_with_help) {
        fields.push_back(field_with_help.first);
        fields_help.push_back(field_with_help.second);
    }

    return make_map_initializer_with_help<BT>(func, fields, fields_help);
}

template <typename BT, typename F>
yadi_info_t<BT> make_map_initializer_with_help(F func, std::vector<std::string> fields,
                                               std::vector<std::string> fields_help) {
    std::vector<std::string> field_types;
    details::yaml_to_tuple<details::function_traits_params_type<F>>::to_arg_types(std::back_inserter(field_types));
    if (field_types.size() != fields.size()) {
        throw std::runtime_error("Field count must match argument count");
    }
    std::string help = "Expects yaml map with fields:";
    for (size_t i = 0; i < fields.size(); ++i) {
        std::string const& field_type = field_types[i];
        std::string const& field = fields[i];
        std::string field_help;
        if (fields_help.size() > i) {
            field_help = fields_help[i];
        }
        help += "\n\t\t";
        help += field + ": " + field_type;
        if (!field_help.empty()) {
            help += ", " + field_help;
        }
    }
    return {make_map_initializer<BT>(func, fields), help};
}

// TODO thread safe
template <typename BT>
initializer_type_t<BT> make_caching_initializer(initializer_type_t<BT> const& initializing_initializer) {
    static_assert(std::is_same<ptr_type_t<BT>, std::shared_ptr<BT>>::value,
                  "ptr_type for BT must be shared_ptr to use caching initializer");

    using Cache = std::map<std::string, std::weak_ptr<BT>>;
    Cache cache;
    auto caching_initializer = [cache, initializing_initializer](YAML::Node const& config) mutable -> ptr_type_t<BT> {
        // TODO Apply formatting to yaml_key
        std::string yaml_key = YAML::Dump(config);
        typename Cache::iterator cache_it = cache.find(yaml_key);
        if (cache_it != cache.end()) {
            ptr_type_t<BT> instance = cache_it->second.lock();
            if (instance) {
                return instance;
            }
        }

        ptr_type_t<BT> new_instance = initializing_initializer(config);
        cache[yaml_key] = new_instance;
        return new_instance;
    };

    return caching_initializer;
}

template <typename BT>
yadi_info_t<BT> make_caching_initializer(yadi_info_t<BT> yi) {
    yi.initializer = make_caching_initializer<BT>(yi.initializer);
    return yi;
}

}  // namespace yadi

#endif  // YADI_INITIALIZERS_HPP
