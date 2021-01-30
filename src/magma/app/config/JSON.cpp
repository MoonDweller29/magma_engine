#include "magma/app/config/JSON.h"
#include "magma/app/log.hpp"

namespace json {

void save(const std::string &filename, const JSON &json) {
    std::ofstream file(filename);
    if (file) {
        file << std::setw(4) << json;
    } else {
        LOG_ERROR("Can't save JSON: can't open file: ", filename);
    }
}

template <>
JSON load<JSON>(const std::string &filename) {
    JSON json;
    std::ifstream file(filename);
    if (file) {
        file >> json;
    } else {
        LOG_ERROR("Can't load JSON from file: ", filename);
    }

    return json;
}

} // namespace json

