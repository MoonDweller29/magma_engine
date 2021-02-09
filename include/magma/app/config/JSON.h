#pragma once

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>


namespace detail {

namespace json {

template <typename T, typename SFINAE>
class Serializer;

} // namespace json

} // namespace detail

using JSON = nlohmann::basic_json<
        /* ObjectType */            std::map,
        /* ArrayType */             std::vector,
        /* StringType */            std::string,
        /* BooleanType */           bool,
        /* NumberIntegerType */     std::int64_t,
        /* NumberUnsignedType */    std::uint64_t,
        /* NumberFloatType */       double,
        /* AllocatorType */         std::allocator,
        /* JSONSerializer */        detail::json::Serializer,
        /* BinaryType */            std::vector<std::uint8_t>
>;


#define JSON_MAPPINGS(...)                                      \
    void to_json(JSON &json) const {                            \
        ::detail::json::ValueWriter mappings[] = {__VA_ARGS__}; \
        for (const auto &mapping : mappings) {                  \
            mapping.apply(json);                                \
        }                                                       \
    }                                                           \
                                                                \
    void from_json(const JSON &json) {                          \
        std::cout << "FROM JSON" << std::endl;                                                        \
        ::detail::json::ValueReader mappings[] = {__VA_ARGS__}; \
        for (const auto &mapping : mappings) {                  \
            mapping.apply(json);                                \
        }                                                       \
    }                                                           \
                                                                \
    template <typename T>                                       \
    friend class ::detail::json::has_mappings;                  \
                                                                \
    template <typename T, typename SFINAE>                      \
    friend class ::detail::json::Serializer;


// @TODO: use map instead of linear search
#define JSON_ENUM_MAPPING(type, ...)                                            \
    inline void to_json(JSON &json, const type &value) {                               \
        static const std::pair<type, std::string> mappings[] = {__VA_ARGS__};   \
        auto mapping = std::find_if(                                            \
            std::begin(mappings),                                               \
            std::end(mappings),                                                 \
            [value](auto pair) { return pair.first == value; }                  \
        );                                                                      \
        if (mapping != std::end(mappings)) {                                    \
            json = mapping->second;                                             \
        } else {                                                                \
            json = nullptr;                                                     \
        }                                                                       \
    }                                                                           \
                                                                                \
    inline void from_json(const JSON &json, type &value) {                             \
        std::cout << json << "LOOOOOL" << std::endl;                                                                        \
        static const std::pair<type, std::string> mappings[] = {__VA_ARGS__};   \
        auto mapping = std::find_if(                                            \
            std::begin(mappings),                                               \
            std::end(mappings),                                                 \
            [&json](auto pair) { return json == pair.second; }                  \
        );                                                                      \
        if (mapping != std::end(mappings)) {                                    \
            value = mapping->first;                                             \
        }                                                                       \
    }


namespace detail {

namespace json {

template <typename T>
class has_mappings {
private:
    template <typename F, F>
    struct has;

    template <typename C>
    static std::true_type hasToJSON(has<void (C::*)(JSON &) const, &C::to_json> *);

    template <typename ...>
    static std::false_type hasToJSON(...);

    template <typename C>
    static std::true_type hasFromJSON(has<void (C::*)(const JSON &), &C::from_json> *);

    template <typename ...>
    static std::false_type hasFromJSON(...);

public:
    static constexpr bool to_json = decltype(hasToJSON<T>(nullptr))::value;
    static constexpr bool from_json = decltype(hasFromJSON<T>(nullptr))::value;

};

template <typename T, typename = void>
struct Serializer {
    static void to_json(JSON &json, const T &object) {
        if constexpr (detail::json::has_mappings<T>::to_json) {
            object.to_json(json);
        } else {
            nlohmann::to_json(json, object);
        }
    }

    static void from_json(const JSON &json, T &object) {
        if constexpr (detail::json::has_mappings<T>::from_json) {
            object.from_json(json);
        } else {
            nlohmann::from_json(json, object);
        }
    }

};

class ValueWriter {
public:
    template <typename T>
    ValueWriter(const T &value, const std::string &field)
        : apply([&value, field](JSON &json) {
            Serializer<T>::to_json(json[field], value);
        }) {

    }

    std::function<void(JSON &)> apply;

};

class ValueReader {
public:
    template <typename T>
    ValueReader(T &value, const std::string &field)
        : apply([&value, field](const JSON &json) {
            JSON::const_iterator property = json.find(field);
            if (property != json.end()) {
                Serializer<T>::from_json(*property, value);
            }
        }) {

    }

    std::function<void(const JSON &)> apply;

};

} // namespace json

} // namespace detail


namespace json {

void save(const std::string &filename, const JSON &json);

template <typename T>
void save(const std::string &filename, const T &object) {
    JSON json;
    detail::json::Serializer<T>::to_json(json, object);
    save(filename, json);
}

template <typename T>
T load(const std::string &filename) {
    T object;
    JSON json = load<JSON>(filename);

    detail::json::Serializer<T>::from_json(json, object);
    return object;
}

template <>
JSON load<JSON>(const std::string &filename);

template <typename T>
void load(const std::string &filename, T &object) {
    JSON json = load<JSON>(filename);
    detail::json::Serializer<T>::from_json(json, object);
}

} // namespace json
