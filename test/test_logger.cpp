#include <magma/app/log.hpp>

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

    TRY_CATCH(LOG_AND_THROW std::logic_error("test: logic err"));
    TRY_CATCH(LOG_AND_THROW std::invalid_argument("test: invalid arg"));
    TRY_CATCH(LOG_AND_THROW std::out_of_range("test: out of range"));
    TRY_CATCH(LOG_AND_THROW std::runtime_error("test: runtime err"));
}
