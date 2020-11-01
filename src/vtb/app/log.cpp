#include "vtb/app/log.hpp"

Log::Config Log::_config;
std::ofstream Log::log_out_fs;
std::stringstream Log::ss;


void Log::init() {
    log_out_fs.open(_config.log_filename, std::ios_base::app);
}

void Log::initFromConfig(const Log::Config &init_config) {
    _config = init_config;
    init();
}

std::ostream &operator<<(std::ostream &stream, Log::Level level) {
    switch (level) {
        case Log::Level::DEBUG:    return stream << "[DEBUG]";
        case Log::Level::INFO:     return stream << "[INFO]";
        case Log::Level::WARNING:  return stream << "[WARNING]";
        case Log::Level::ERROR:    return stream << "[ERROR]";
        case Log::Level::CRITICAL: return stream << "[CRITICAL]";
        default:                   return stream;
    }
}

bool operator<=(Log::Level l1, Log::Level l2) {
    return static_cast<int>(l1) <= static_cast<int>(l2);
}
bool operator>=(Log::Level l1, Log::Level l2) {
    return static_cast<int>(l1) >= static_cast<int>(l2);
}
bool operator<(Log::Level l1, Log::Level l2) {
    return static_cast<int>(l1) < static_cast<int>(l2);
}
bool operator>(Log::Level l1, Log::Level l2) {
    return static_cast<int>(l1) > static_cast<int>(l2);
}
