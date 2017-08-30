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

}  // namespace yadi

#endif  // YADI_FACTORY_HPP__
