#include <app/log.hpp>

struct NonPrintable {};

void test_logger() {
    Log::Config config {
        "default.log",
        true,
        Log::Level::DEBUG
    };
    Log::initFromConfig(config);
//    Log::initFromConfig("log.config");

    NonPrintable test_object {};
    // TODO: testing infrastructure
    LOG_DEBUG("This is debug logging message, ", 2);
    LOG_INFO("Important information about non-printable struct: ", test_object);
    LOG_WARNING("A warning");
    LOG_ERROR("An error!");
    LOG_CRITICAL("A critical error!!!");
}