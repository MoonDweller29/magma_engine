#include <app/logger.hpp>

struct NonPrintable {};

void test_logger() {
    NonPrintable test_object {};
    // TODO: testing infrastructure
    log::debug("This is debug logging message, ", 2);
    log::info("Important information about non-printable struct: ", test_object);
    log::warning("A warning");
    log::error("An error!");
    log::critical("A critical error!!!");
}