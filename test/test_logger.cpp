#include <app/log.hpp>

struct NonPrintable {};

void test_logger() {
    NonPrintable test_object {};
    Log::initFromConfig("log.config");
    // TODO: testing infrastructure
    LOG_DEBUG("This is debug logging message, ", 2);
    LOG_INFO("Important information about non-printable struct: ", test_object);
    LOG_WARNING("A warning");
    LOG_ERROR("An error!");
    LOG_CRITICAL("A critical error!!!");
}