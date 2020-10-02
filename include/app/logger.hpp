#pragma once

#include <iostream>

namespace log {
    enum class Level {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL

    };

    std::ostream &operator<<(std::ostream &stream, Level level) {
        switch (level) {
            case Level::DEBUG:    return stream << "[DEBUG]";
            case Level::INFO:     return stream << "[INFO]";
            case Level::WARNING:  return stream << "[WARNING]";
            case Level::ERROR:    return stream << "[ERROR]";
            case Level::CRITICAL: return stream << "[CRITICAL]";
        }
    }

    namespace detail {
        template <typename T>
        auto print(std::ostream &stream, const T &object) -> decltype((stream << object), void()) {
            stream << object;
        }

        template <typename T>
        void print(std::ostream &stream, T &object) {
            stream << &object;
        }

    } // namespace detail

    template <typename ... Args>
    void message(Level level, const Args &... args) {
        // TODO: output to log file
        // TODO: log timestamp
        // TODO: choose message format
        // TODO: add filtering by level
        std::cerr << level << ' ';
        (detail::print(std::cerr, args), ...);
        std::cerr << std::endl;
    }

    template <typename ... Args>
    void debug(const Args &... args) {
        message(Level::DEBUG, args...);
    }

    template <typename ... Args>
    void info(const Args &... args) {
        message(Level::INFO, args...);
    }

    template <typename ... Args>
    void warning(const Args &... args) {
        message(Level::WARNING, args...);
    }

    template <typename ... Args>
    void error(const Args &... args) {
        message(Level::ERROR, args...);
    }

    template <typename ... Args>
    void critical(const Args &... args) {
        message(Level::CRITICAL, args...);
    }

} // namespace log