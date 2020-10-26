#pragma once

#include <iostream>
#include <type_traits>
#include <string>
#include <sstream>
#include <app/clock.h>
#include "app/config/JSON.h"

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

    struct Config {
        std::string   log_filename       =  "default_log.log";
        bool          write_to_console   =  true;
        int           minimal_level      =  0;
    private:
        JSON_MAPPINGS(
            {log_filename, "filename"},
            {write_to_console, "write_to_console"},
            {minimal_level, "minimal_level"},
        )
    };

    static void initFromConfig(const Log::Config &init_config);
    static void initFromConfig(const std::string &filename);

    friend std::ostream &operator<<(std::ostream &stream, Log::Level level);

    template <typename ... Args>
    static void message(Level level, const Args &... args);

    template <typename ... Args>
    static void debug(const Args &... args);

    template <typename ... Args>
    static void info(const Args &... args);

    template <typename ... Args>
    static void warning(const Args &... args);

    template <typename ... Args>
    static void error(const Args &... args);

    template <typename ... Args>
    static void critical(const Args &... args);

private:
    static Config _config;
    static std::ofstream log_out_fs;

    static void init();

    template <typename T>
    static auto has_output_operator(std::ostream &stream, const T &object)
        -> decltype(stream << object, std::true_type());

    template <typename ...>
    static auto has_output_operator(...)
        -> std::false_type;

    template <typename T>
    static void print(std::ostream &stream, const T &object);
};

// TODO: move to common include file maybe
#ifndef __FILENAME__
#define __FILENAME__ __FILE__
#endif

#define LOG_DEBUG(...)    Log::debug    ("[", __FILENAME__, ":", __LINE__, "] ", __VA_ARGS__)
#define LOG_INFO(...)     Log::info     ("[", __FILENAME__, ":", __LINE__, "] ", __VA_ARGS__)
#define LOG_WARNING(...)  Log::warning  ("[", __FILENAME__, ":", __LINE__, "] ", __VA_ARGS__)
#define LOG_ERROR(...)    Log::error    ("[", __FILENAME__, ":", __LINE__, "] ", __VA_ARGS__)
#define LOG_CRITICAL(...) Log::critical ("[", __FILENAME__, ":", __LINE__, "] ", __VA_ARGS__)

template <typename ... Args>
void Log::message(Level level, const Args &... args) {
    // TODO: output to log file
    // TODO: choose message format
    // TODO: add filtering by level
    std::stringstream ss;
    ss << "[" << SystemClock::getTime() << "] " << level << ' ';
    (print<Args>(ss, args), ...);
    ss << std::endl;
    if (_config.write_to_console) {
        std::cerr << ss.str();
    }
    if (log_out_fs.is_open()) {
        log_out_fs << ss.str();
    }
}

template <typename ... Args>
void Log::debug(const Args &... args) {
    if (static_cast<int>(_config.minimal_level) <= static_cast<int>(Level::DEBUG)) {
        message(Level::DEBUG, args...);
    }
}

template <typename ... Args>
void Log::info(const Args &... args) {
    if (static_cast<int>(_config.minimal_level) <= static_cast<int>(Level::INFO)) {
        message(Level::INFO, args...);
    }
}

template <typename ... Args>
void Log::warning(const Args &... args) {
    if (static_cast<int>(_config.minimal_level) <= static_cast<int>(Level::WARNING)) {
        message(Level::WARNING, args...);
    }
}

template <typename ... Args>
void Log::error(const Args &... args) {
    if (static_cast<int>(_config.minimal_level) <= static_cast<int>(Level::ERROR)) {
        message(Level::ERROR, args...);
    }
}

template <typename ... Args>
void Log::critical(const Args &... args) {
    if (static_cast<int>(_config.minimal_level) <= static_cast<int>(Level::CRITICAL)) {
        message(Level::CRITICAL, args...);
    }
}

template <typename T>
void Log::print(std::ostream &stream, const T &object) {
    if constexpr (decltype(has_output_operator(stream, object))::value) {
        stream << object;
    } else {
        stream << &object;
    }
}