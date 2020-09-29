#pragma once

#include <fstream>
#include <iomanip>

#include <nlohmann/json.hpp>


namespace detail {
    template <typename T, typename SFINAE>
    class JSONSerializer;
}

using json = nlohmann::basic_json<
        /* ObjectType */            std::map,
        /* ArrayType */             std::vector,
        /* StringType */            std::string,
        /* BooleanType */           bool,
        /* NumberIntegerType */     std::int64_t,
        /* NumberUnsignedType */    std::uint64_t,
        /* NumberFloatType */       double,
        /* AllocatorType */         std::allocator,
        /* JSONSerializer */        detail::JSONSerializer,
        /* BinaryType */            std::vector<std::uint8_t>
>;


#define JSON_MAPPINGS(...)                                  \
    void to_json(json &jsn) const {                         \
        detail::JSONValueWriter mappings[] = {__VA_ARGS__}; \
        for (const auto &mapping : mappings) {              \
            mapping.apply(jsn);                             \
        }                                                   \
    }                                                       \
                                                            \
    void from_json(const json &jsn) {                       \
        detail::JSONValueReader mappings[] = {__VA_ARGS__}; \
        for (const auto &mapping : mappings) {              \
            mapping.apply(jsn);                             \
        }                                                   \
    }                                                       \
                                                            \
    template <typename T>                                   \
    friend class ::detail::has_json_mappings;               \
                                                            \
    template <typename T, typename SFINAE>                  \
    friend class ::detail::JSONSerializer;


namespace detail {
    template <typename T>
    class has_json_mappings {
    private:
        template <typename F, F>
        struct has;

        template <typename C>
        static std::true_type has_to_json(has<void (C:: *)(json &) const, &C::to_json> *);

        template <typename ...>
        static std::false_type has_to_json(...);

        template <typename C>
        static std::true_type has_from_json(has<void (C:: *)(const json &), &C::from_json> *);

        template <typename ...>
        static std::false_type has_from_json(...);

    public:
        static constexpr bool to_json   = decltype(has_to_json<T>(nullptr))::value;
        static constexpr bool from_json = decltype(has_from_json<T>(nullptr))::value;

    };

    template<typename T, typename = void>
    struct JSONSerializer {
        static void to_json(json &jsn, const T &object) {
            if constexpr (detail::has_json_mappings<T>::to_json) {
                object.to_json(jsn);
            } else {
                nlohmann::to_json(jsn, object);
            }
        }

        static void from_json(const json &jsn, T &object) {
            if constexpr (detail::has_json_mappings<T>::from_json) {
                object.from_json(jsn);
            } else {
                nlohmann::from_json(jsn, object);
            }
        }

    };

    class JSONValueWriter {
    public:
        template <typename T>
        JSONValueWriter (const T &value, const std::string &field)
            : apply([&value, field](json &jsn) {
                JSONSerializer<T>::to_json(jsn[field], value);
            }) {

        }

        std::function<void(json &)> apply;

    };

    class JSONValueReader {
    public:
        template <typename T>
        JSONValueReader(T &value, const std::string &field)
            : apply([&value, field](const json &jsn) {
                json::const_iterator property = jsn.find(field);
                if (property != jsn.end()) {
                    JSONSerializer<T>::from_json(*property, value);
                }
            }) {

        }

        std::function<void(const json &)> apply;

    };

} // namespace detail


template <typename T>
void save_as_json(const std::string &filename, const T &object) {
    json jsn;
    detail::JSONSerializer<T>::to_json(jsn, object);
    std::ofstream file(filename);
    file << std::setw(4) << jsn;
}

template <typename T>
void load_from_json(const std::string &filename, T &object) {
    json jsn;
    std::ifstream file(filename);
    file >> jsn;
    detail::JSONSerializer<T>::from_json(jsn, object);
}

template <typename T>
T load_from_json(const std::string &filename) {
    json jsn;
    T object;
    std::ifstream file(filename);
    file >> jsn;
    detail::JSONSerializer<T>::from_json(jsn, object);
    return object;
}
