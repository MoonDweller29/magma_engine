#pragma once

#include <iostream>
#include <type_traits>

class Log {
public:
    Log() = delete;

    enum class Level {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL
    };

    friend std::ostream &operator<<(std::ostream &stream, Log::Level level) {
        switch (level) {
            case Log::Level::DEBUG:    return stream << "[DEBUG]";
            case Log::Level::INFO:     return stream << "[INFO]";
            case Log::Level::WARNING:  return stream << "[WARNING]";
            case Log::Level::ERROR:    return stream << "[ERROR]";
            case Log::Level::CRITICAL: return stream << "[CRITICAL]";
            default:                   return stream;
        }
    }

    template <typename ... Args>
    static void message(Level level, const Args &... args) {
        // TODO: output to log file
        // TODO: log timestamp
        // TODO: choose message format
        // TODO: add filtering by level
        std::cerr << level << ' ';
        (print<Args>(std::cerr, args), ...);
        std::cerr << std::endl;
    }

    template <typename ... Args>
    static void debug(const Args &... args) {
        message(Level::DEBUG, args...);
    }

    template <typename ... Args>
    static void info(const Args &... args) {
        message(Level::INFO, args...);
    }

    template <typename ... Args>
    static void warning(const Args &... args) {
        message(Level::WARNING, args...);
    }

    template <typename ... Args>
    static void error(const Args &... args) {
        message(Level::ERROR, args...);
    }

    template <typename ... Args>
    static void critical(const Args &... args) {
        message(Level::CRITICAL, args...);
    }

private:
    template <typename T>
    static auto has_output_operator(std::ostream &stream, const T &object)
        -> decltype(stream << object, std::true_type());

    template <typename ...>
    static auto has_output_operator(...)
        -> std::false_type;

    template <typename T>
    static void print(std::ostream &stream, const T &object) {
        if constexpr (decltype(has_output_operator(stream, object))::value) {
            stream << object;
        } else {
            stream << &object;
        }
    }

};
