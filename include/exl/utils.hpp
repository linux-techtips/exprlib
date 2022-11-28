#pragma once

#include <exl/types.hpp>
#include <csignal>

namespace exl {

    inline fn abort() noexcept -> void {
        std::raise(SIGABRT);
    }

    inline fn assert(const bool expr) noexcept -> void {
        if (!expr) { abort(); }
    }

    template <bool>
    struct comptime_assert_t; 

    template <>
    struct comptime_assert_t<true>{};

    #define comptime_assert_id_unwrap(base, line, counter) base ## line ## counter
    #define comptime_assert_id(base, line, file) comptime_assert_id_unwrap(base, line, counter)
    #define comptime_assert(expr) comptime_assert_t comptime_assert_id(__comptime_assert_, __LINE__, __COUNTER__) = exl::comptime_assert_t<(expr)>{};

} // namespace exl