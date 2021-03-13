#pragma once

#include <iostream>
#include <type_traits>
#include <string>
#include <string_view>
#include <sstream>
#include "magma/app/clock.h"
#include "magma/app/config/JSON.h"

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
        bool          write_to_file      =  false;
        Level         minimal_level      =  Level::DEBUG;
    private:
        JSON_MAPPINGS(
            {log_filename, "output_filename"},
            {write_to_console, "write_to_console"},
            {write_to_file, "write_to_file"},
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

    static std::string location(std::string_view filename, unsigned line);

    class ExceptionLogger {
    public:
        ExceptionLogger(const char *filename, int line);

        template <typename E>
        [[ noreturn ]] void operator<<(E exception) const;

    private:
        const char *_filename;
        int _line;

    };

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

#define LOG_DEBUG(...)    Log::debug    (Log::location(__FILENAME__, __LINE__), __VA_ARGS__)
#define LOG_INFO(...)     Log::info     (Log::location(__FILENAME__, __LINE__), __VA_ARGS__)
#define LOG_WARNING(...)  Log::warning  (Log::location(__FILENAME__, __LINE__), __VA_ARGS__)
#define LOG_ERROR(...)    Log::error    (Log::location(__FILENAME__, __LINE__), __VA_ARGS__)
#define LOG_CRITICAL(...) Log::critical (Log::location(__FILENAME__, __LINE__), __VA_ARGS__)
#define LOG_AND_THROW Log::ExceptionLogger(__FILENAME__, __LINE__) <<


template <typename ... Args>
void Log::message(Level level, const Args &... args) {
    // TODO: choose message format
    if (level < _config.minimal_level) {
        return;
    }
    ss.str("");
    ss << "[" << SystemClock::getTime() << "] " << level << ' ';
    (print<Args>(ss, args), ...);
    if (_config.write_to_console) {
        std::cerr << ss.str() << std::endl;
    }
    if (log_out_fs.is_open() && _config.write_to_file) {
        log_out_fs << ss.str() << std::endl;
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


template <typename E>
void Log::ExceptionLogger::operator<<(E exception) const {
    Log::error(Log::location(_filename, _line), exception.what());
    throw exception;
}
