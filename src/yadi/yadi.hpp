#ifndef YADI_FACTORY_HPP__
#define YADI_FACTORY_HPP__

#include <yaml-cpp/yaml.h>

#include <functional>
#include <map>
#include <memory>
#include <string>

namespace yadi {

/**
 * @brief Factory traits that can be changed for base_t.
 * @tparam base_t
 */
template <typename base_t>
struct factory_traits {
    using ptr_type = std::unique_ptr<base_t>;  /// The type of pointer to return from create.
};

/**
 * @brief The type of pointer the base_t factory creates.
 */
template <typename base_t>
using ptr_type_t = typename factory_traits<base_t>::ptr_type;

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

// ################# IMPL ################################
template <typename base_t>
void factory<base_t>::register_type(std::string type, initializer_type initializer) {
    mut_types()[type].initializer = initializer;
}

template <typename base_t>
typename factory<base_t>::ptr_type factory<base_t>::create(std::string const& type,
                                                           YAML::Node const& config) {
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

template <typename base_t, typename impl_t>
ptr_type_t<base_t> yaml_init(YAML::Node const& config) {
    ptr_type_t<base_t> ret(new impl_t(config));
    return ret;
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
    register_type<base_t>(type, [](YAML::Node) {
        ptr_type_t<base_t> p(new impl_t);
        return p;
    });
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

}  // namespace yadi

#endif  // YADI_FACTORY_HPP__
