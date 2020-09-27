#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>


using json = nlohmann::json;


template <typename T, typename V>
class JSONMapping;

template <typename T, typename ... V>
class JSONMappings : public std::tuple<JSONMapping<T, V>...> {
public:
    static constexpr std::size_t mappings_count = sizeof...(V);

    explicit JSONMappings(JSONMapping<T, V> &&... mappings)
        : std::tuple<JSONMapping<T, V>...>(std::move(mappings)...) {

    }

};

template <>
class JSONMappings<void> {
public:
    static constexpr std::size_t mappings_count = 0;

};


template <typename ... T, typename ... V>
auto define_mappings(JSONMapping<T, V> &&... mappings) {
    return JSONMappings(std::move(mappings)...);
}


template <typename T>
const auto json_mappings = JSONMappings<void>();


namespace detail {
    template <typename T>
    using MappingsType = decltype(json_mappings<std::decay_t<T>>);

    template <typename T>
    constexpr auto &get_mappings() {
        static_assert(
                MappingsType<T>::mappings_count != 0,
                "JSON mappings are not defined for the given type!"
        );
        return json_mappings<std::decay_t<T>>;
    }

    template <typename T>
    constexpr bool has_mappings() {
        return MappingsType<T>::mappings_count != 0;
    }

    template <typename T>
    constexpr std::size_t get_mappings_count() {
        return MappingsType<T>::mappings_count;
    }

    template <typename T, std::size_t... I>
    void to_json_impl(json &jsn, const T &object, std::index_sequence<I...> /* indices */) {
        (std::get<I>(get_mappings<T>()).to_json(jsn, object), ...);
    }

    template <typename T, std::size_t... I>
    void from_json_impl(const json &jsn, T &object, std::index_sequence<I...> /* indices */) {
        (std::get<I>(get_mappings<T>()).from_json(jsn, object), ...);
    }

}


template <typename T>
std::enable_if_t<detail::has_mappings<T>()> to_json(json &jsn, const T &object) {
    detail::to_json_impl(
            jsn,
            object,
            std::make_index_sequence<detail::get_mappings_count<T>()>()
    );
}

template <typename T>
std::enable_if_t<detail::has_mappings<T>()> from_json(const json &jsn, T &object) {
    detail::from_json_impl(
            jsn,
            object,
            std::make_index_sequence<detail::get_mappings_count<T>()>()
    );
}


enum JSONSkipIfNotChanged {
    SKIP_IF_NOT_CHANGED
};


template <typename T, typename V>
class JSONMapping {
public:
    template <typename D = const V &>
    JSONMapping(
        V T::*                                  object_property,
        std::string_view                        json_field,
        D                                       default_value,
        [[maybe_unused]] JSONSkipIfNotChanged   skip_if_not_changed)
            : m_property(object_property)
            , m_field(json_field)
            , m_default_value(std::forward<D>(default_value))
            , m_comparator(+[](const V &a, const V &b) -> bool { return a == b; }) {

    }

    template <typename D = const V &>
    JSONMapping(
            V T::*              object_property,
            std::string_view    json_field,
            D                   default_value = V())
            : m_property(object_property)
            , m_field(json_field)
            , m_default_value(std::forward<D>(default_value))
            , m_comparator(nullptr) {

    }

    void to_json(json &jsn, const T &object) const {
        const V &value = object.*m_property;
        if ((m_comparator == nullptr) || !m_comparator(value, m_default_value)) {
            jsn[m_field] = value;
        }
    }

    void from_json(const json &jsn, T &object) const {
        V &value = object.*m_property;
        json::const_iterator field = jsn.find(m_field);
        value = (field != jsn.end()) ? field->get<V>() : m_default_value;
    }

private:
    V T::*      m_property;
    std::string m_field;
    V m_default_value;
    bool (*m_comparator)(const V &, const V &);

};
