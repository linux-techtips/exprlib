#include "exl/errs.hpp"
#include "exl/utils.hpp"
#include <exl/core.hpp>
#include <fmt/core.h>

fn test_types() -> void {
    fmt::print("[STATUS] Testing types...\n");
    exl::comptime_assert(sizeof(s8) == 1);
    exl::comptime_assert(sizeof(u8) == 1);
    exl::comptime_assert(sizeof(s16) == 2);
    exl::comptime_assert(sizeof(u16) == 2);
    exl::comptime_assert(sizeof(s32) == 4);
    exl::comptime_assert(sizeof(u32) == 4);
    exl::comptime_assert(sizeof(s64) == 8);
    exl::comptime_assert(sizeof(u64) == 8);
    exl::comptime_assert(sizeof(ssize) == 8);
    exl::comptime_assert(sizeof(usize) == 8);
}

fn test_helper_result() -> exl::Result<const char*, const char*> {
    //return exl::Ok<const char*, const char*>("Hellol");
}

fn test_result() -> void {
    fmt::print("[STATUS] Testing result...\n");
    //fmt::print("{}\n", test_helper_result().ok);
}

fn main() -> int {
    fmt::print("[STATUS] Begining test...\n\n");

    test_types();
    test_result();
    
    fmt::print("\n[SUCCESS] Test complete!\n");
}