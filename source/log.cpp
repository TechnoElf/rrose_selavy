/*
 * Created by janis on 2024-01-18
 */

#include "log.h"

#include <iostream>

namespace rr {
    void Log::error(const char* msg) {
        std::cerr << "[RR] Error: " << msg << std::endl;
    }

    void Log::info(const char* msg) {
        std::cerr << "[RR] Info: " << msg << std::endl;
    }
}
