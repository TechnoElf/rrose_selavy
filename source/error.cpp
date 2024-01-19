/*
 * Created by janis on 2024-01-18
 */

#include "error.h"

namespace rr {
    const char* to_string(ErrorKind v) {
        switch (v) {
            case ErrorKind::Vulkan: return "Vulkan";
            case ErrorKind::Unknown: return "Unknown";
        }
    }

    std::string Error::to_string() {
        std::string msg;

        msg += rr::to_string(this->kind);

        return msg;
    }
}

