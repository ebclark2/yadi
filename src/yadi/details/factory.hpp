//
// Created by Ed Clark on 8/29/17.
//

#ifndef YADI_FACTORY_HPP
#define YADI_FACTORY_HPP

#include "demangle.hpp"

#include <yaml-cpp/yaml.h>

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <type_traits>

#ifdef YADI_DEBUG
#include <iostream>
#endif

// TODO Find a better home for these macros
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
    struct yadi::factory_traits<TYPE> {               \
        using ptr_type = TYPE;                        \
        static const bool direct_from_yaml = true;    \
    };                                                \
    extern template struct ::yadi::factory<TYPE>;

#define YADI_YAML_TYPE_BY_VALUE_DEFN(TYPE, INIT_NAME)                                            \
    template struct ::yadi::factory<TYPE>;                                                       \
    YADI_INIT_BEGIN_N(INIT_NAME)                                                                 \
    ::yadi::register_type<TYPE>(::yadi::type_by_value_key(), ::yadi::yaml_as_with_help<TYPE>()); \
    ::yadi::register_factory<TYPE>(#TYPE);                                                       \
    YADI_INIT_END_N(INIT_NAME)

namespace yadi {

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

// TODO YADI_DECL and YADI_DEFN macros with bit to add factory type to something for help retrieval
template <typename BT>
struct factory {
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

template <typename BT>
using initializer_type_t = typename factory<BT>::initializer_type;

// TODO Comment here
template <typename BT>
using yadi_info_t = typename factory<BT>::yadi_info;

// ############################ IMPL ############################

template <typename BT>
void factory<BT>::register_type(std::string type, yadi_info yadis) {
#ifdef YADI_DEBUG
    std::cerr << "Registering \"" + type + "\" to \"" + demangle_type<BT>() + "\" factory\n";
#endif
    mut_types()[type] = yadis;
}

template <typename BT>
typename factory<BT>::ptr_type factory<BT>::create(std::string const& type, YAML::Node const& config) {
    typename type_store::const_iterator type_iter = mut_types().find(type);
    if (type_iter == mut_types().end()) {
        throw std::runtime_error("\"" + type + "\" not found in \"" + demangle_type<BT>() + "\" factory");
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

}  // namespace yadi

#endif  // YADI_FACTORY_HPP
