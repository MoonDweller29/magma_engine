/**
 * @file Option.hpp
 * @brief Container for required or recommended option
 * @version 0.1
 * @date 2021-02-18
 */
#pragma once
#include <optional>

template <typename T>
class Option {
public:
    Option() = default;

    static Option<T> Requirement(const T &value);
    static Option<T> Recommendation(const T &value);

    void require(const T& value);
    void recommend(const T& value);

    bool isRequired()    const { return _isRequired; }
    bool isRecommended() const { return _value.has_value() && !_isRequired;}

    const T &getValue() const { return _value.value(); }
private:
    Option(const T &value, bool required);
    std::optional<T> _value;
    bool _isRequired;
};

template <typename T>
Option<T>::Option(const T &value, bool required):
    _value(value), _isRequired(required)
{}

template <typename T>
Option<T> Option<T>::Requirement(const T& value) {
    return Option(value, true);
}

template <typename T>
Option<T> Option<T>::Recommendation(const T& value) {
    return Option(value, false);
}

template <typename T>
void Option<T>::require(const T& value) {
    _value = value;
    _isRequired = true;
}

template <typename T>
void Option<T>::recommend(const T& value) {
    _value = value;
    _isRequired = false;
}