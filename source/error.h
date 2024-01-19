/*
 * Created by janis on 2024-01-18
 */

#pragma once

#include <expected>
#include <string>

#include "log.h"

namespace rr {
    enum class ErrorKind {
        Vulkan,
        Unknown
    };

    struct Error {
        ErrorKind kind;

    public:
        std::string to_string();
    };

    template<typename R>
    struct Result {
        std::expected<R, Error> v;

    public:
        Result(R&& success) : v(std::expected<R, Error>(std::move(success))) {}
        Result(Error&& error) : v(std::unexpected<Error>(std::move(error))) {}

        R unwrap() && {
            if (this->v.has_value()) {
                return std::move(*this->v);
            } else {
                Log::error(this->v.error().to_string().c_str());
                abort();
            }
        }
    };
}