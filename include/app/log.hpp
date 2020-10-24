#pragma once

#include <iostream>
#include <type_traits>
#include <string>
#include <array>
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
private:
    struct Params {
        std::string log_filename = "default_log.log";
        bool write_to_console = true;
        int minimal_level =  0;
    private:
        JSON_MAPPINGS(
            {log_filename, "filename"},
            {write_to_console, "write_to_console"},
            {minimal_level, "minimal_level"},
        )
    };
    static Params params;
    static std::ofstream log_out_fs;
public:

    static void initFromConfig(const std::string &filename) {
        json::load<Params>(filename, params);
        log_out_fs.open(params.log_filename, std::ios_base::app);
    }

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
        if (params.write_to_console) {
            std::cerr << level << ' ';
            (print<Args>(std::cerr, args), ...);
            std::cerr << std::endl;
            if (log_out_fs.is_open()) {
                log_out_fs << level << ' ';
                (print<Args>(log_out_fs, args), ...);
                log_out_fs << std::endl;
            }
        }
    }

    template <typename ... Args>
    static void debug(const Args &... args) {
        if (static_cast<int>(params.minimal_level) <= static_cast<int>(Level::DEBUG)) {
            message(Level::DEBUG, args...);
        }
    }

    template <typename ... Args>
    static void info(const Args &... args) {
        if (static_cast<int>(params.minimal_level) <= static_cast<int>(Level::INFO)) {
            message(Level::INFO, args...);
        }
    }

    template <typename ... Args>
    static void warning(const Args &... args) {
        if (static_cast<int>(params.minimal_level) <= static_cast<int>(Level::WARNING)) {
            message(Level::WARNING, args...);
        }
    }

    template <typename ... Args>
    static void error(const Args &... args) {
        if (static_cast<int>(params.minimal_level) <= static_cast<int>(Level::ERROR)) {
            message(Level::ERROR, args...);
        }
    }

    template <typename ... Args>
    static void critical(const Args &... args) {
        if (static_cast<int>(params.minimal_level) <= static_cast<int>(Level::CRITICAL)) {
            message(Level::CRITICAL, args...);
        }
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

// TODO: move to common include file maybe
#ifndef __FILENAME__
#define __FILENAME__ __FILE__
#endif

#define LOG_DEBUG(...)    Log::debug    ("[", __FILENAME__, ":", __LINE__, "] ", __VA_ARGS__)
#define LOG_INFO(...)     Log::info     ("[", __FILENAME__, ":", __LINE__, "] ", __VA_ARGS__)
#define LOG_WARNING(...)  Log::warning  ("[", __FILENAME__, ":", __LINE__, "] ", __VA_ARGS__)
#define LOG_ERROR(...)    Log::error    ("[", __FILENAME__, ":", __LINE__, "] ", __VA_ARGS__)
#define LOG_CRITICAL(...) Log::critical ("[", __FILENAME__, ":", __LINE__, "] ", __VA_ARGS__)

Log::Params Log::params;
std::ofstream Log::log_out_fs;
