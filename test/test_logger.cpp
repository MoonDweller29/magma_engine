#include <magma/app/log.hpp>
#include <magma/app/errors.h>

struct NonPrintable {};

#define TRY_CATCH(code) try { code; } catch (...) {}

void test_logger() {
    Log::Config config {
        "default.log",
        true,
        true,
        Log::Level::DEBUG
    };
    Log::initFromConfig(config);
//    Log::initFromConfig(json::load<Log::Config>("log.config"));

    NonPrintable test_object {};
    // TODO: testing infrastructure
    LOG_DEBUG("This is debug logging message, ", 2);
    LOG_INFO("Important information about non-printable struct: ", test_object);
    LOG_WARNING("A warning");
    LOG_ERROR("An error!");
    LOG_CRITICAL("A critical error!!!");

    TRY_CATCH(LOGIC_ERR("test: logic err"));
    TRY_CATCH(INVALID_ARG("test: invalid arg"));
    TRY_CATCH(OUT_OF_RANGE_ERR("test: out of range"));
    TRY_CATCH(RUNTIME_ERR("test: runtime err"));
}
