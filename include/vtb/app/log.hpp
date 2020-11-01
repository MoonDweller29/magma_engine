#pragma once

#include <iostream>
#include <type_traits>
#include <string>
#include <sstream>
#include "vtb/app/clock.h"
#include "vtb/app/config/JSON.h"

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
        Level         minimal_level      =  Level::DEBUG;
    private:
        JSON_MAPPINGS(
            {log_filename, "filename"},
            {write_to_console, "write_to_console"},
            {minimal_level, "minimal_level"},
        )
    };

    static void initFromConfig(const Log::Config &init_config);

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
    static std::stringstream ss;

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

bool operator<=(Log::Level l1, Log::Level l2);
bool operator>=(Log::Level l1, Log::Level l2);
bool operator<(Log::Level l1, Log::Level l2);
bool operator>(Log::Level l1, Log::Level l2);

JSON_ENUM_MAPPING(Log::Level,
    { Log::Level::DEBUG,    "DEBUG"    },
    { Log::Level::INFO,     "INFO"     },
    { Log::Level::WARNING,  "WARNING"  },
    { Log::Level::ERROR,    "ERROR"    },
    { Log::Level::CRITICAL, "CRITICAL" }
)

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
    // TODO: choose message format
    if (level < _config.minimal_level) {
        return;
    }
    ss.str("");
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
    message(Level::DEBUG, args...);
}

template <typename ... Args>
void Log::info(const Args &... args) {
    message(Level::INFO, args...);
}

template <typename ... Args>
void Log::warning(const Args &... args) {
    message(Level::WARNING, args...);
}

template <typename ... Args>
void Log::error(const Args &... args) {
    message(Level::ERROR, args...);
}

template <typename ... Args>
void Log::critical(const Args &... args) {
    message(Level::CRITICAL, args...);
}

template <typename T>
void Log::print(std::ostream &stream, const T &object) {
    if constexpr (decltype(has_output_operator(stream, object))::value) {
        stream << object;
    } else {
        stream << &object;
    }
}
