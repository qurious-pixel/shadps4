#pragma once

#include <cstdint>

using s8 = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using f32 = float;
using f64 = double;

#define PS4_MS_ABI __attribute__((ms_abi))
#define PS4_SYSV_ABI __attribute__((sysv_abi))

// UDLs for memory size values
constexpr unsigned long long operator""_KB(unsigned long long x) {
    return 1024ULL * x;
}
constexpr unsigned long long operator""_MB(unsigned long long x) {
    return 1024_KB * x;
}
constexpr unsigned long long operator""_GB(unsigned long long x) {
    return 1024_MB * x;
}
