#pragma once

#include <fstream>
#include <iomanip>
#include <string>
#include <type_traits>

#include <nlohmann/json.hpp>


using json = nlohmann::json;


class JSONMapping {
public:
    template <typename T>
    JSONMapping(T &value, const std::string &field)
        : store()
        , load([&value, field](const json &jsn) {
            json::const_iterator property = jsn.find(field);
            if (property != jsn.end()) {
                value = property->get<T>();
            }
        }) {

    }

    template <typename T>
    JSONMapping(const T &value, const std::string &field)
            : store([&value, field](json &jsn) {
                jsn[field] = value;
            })
            , load() {

    }

    std::function<void(json &)>       store;
    std::function<void(const json &)> load;

};


#define JSON_MAPPINGS(...) \
    void to_json(json &jsn) const {             \
        JSONMapping mappings[] = {__VA_ARGS__}; \
        for (JSONMapping &mapping : mappings) { \
            mapping.store(jsn);                 \
        }                                       \
    }                                           \
                                                \
    void from_json(const json &jsn) {           \
        JSONMapping mappings[] = {__VA_ARGS__}; \
        for (JSONMapping &mapping : mappings) { \
            mapping.load(jsn);                  \
        }                                       \
    }


template <typename T>
void save_as_json(const std::string &filename, const T &object) {
    json jsn;
    to_json(jsn, object);
    std::ofstream file(filename);
    file << std::setw(4) << jsn;
    file.close();
}

template <typename T>
void load_from_json(const std::string &filename, T &object) {
    json jsn;
    std::ifstream file(filename);
    file >> jsn;
    file.close();
    from_json(jsn, object);
}


namespace detail {
    template <typename T>
    class has_json_mappings {
    private:
        template <typename U, U>
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
}


template <typename T>
std::enable_if_t<detail::has_json_mappings<T>::to_json> to_json(json &jsn, const T &object) {
    object.to_json(jsn);
}

template <typename T>
std::enable_if_t<detail::has_json_mappings<T>::from_json> from_json(const json &jsn, T &object) {
    object.from_json(jsn);
}
