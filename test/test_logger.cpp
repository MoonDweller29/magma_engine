#include <app/log.hpp>

struct NonPrintable {};

void test_logger() {
    NonPrintable test_object {};
    // TODO: testing infrastructure
    Log::debug("This is debug logging message, ", 2);
    Log::info("Important information about non-printable struct: ", test_object);
    Log::warning("A warning");
    Log::error("An error!");
    Log::critical("A critical error!!!");
}