#pragma once
#include <stdexcept>
#include "magma/app/log.hpp"


#define LOGIC_ERR(message)              \
{                                       \
    LOG_ERROR(message);                 \
    throw std::logic_error(message);    \
}

#define INVALID_ARG(message)               \
{                                          \
    LOG_ERROR(message);                    \
    throw std::invalid_argument(message);  \
}

#define OUT_OF_RANGE_ERR(message)       \
{                                       \
    LOG_ERROR(message);                 \
    throw std::out_of_range(message);   \
}

#define RUNTIME_ERR(message)            \
{                                       \
    LOG_ERROR(message);                 \
    throw std::runtime_error(message);  \
}
