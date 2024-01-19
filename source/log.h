/*
 * Created by janis on 2024-01-18
 */

#pragma once

namespace rr {
    struct Log {
        static void error(const char* msg);
        static void info(const char* msg);
    };
}