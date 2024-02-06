/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 * Some modifications for using with shadps4 by georgemoralis
 */

#pragma once

#include "types.h"

// Swap endianness macros
#ifdef _MSC_VER
#define SE16(val) _byteswap_ushort(val)
#define SE32(val) _byteswap_ulong(val)
#define SE64(val) _byteswap_uint64(val)
#define SE128(val)                                                                                 \
    U128 {                                                                                         \
        SE64((val).hi), SE64((val).lo)                                                             \
    }
#else
#define SE16(val) __builtin_bswap16(val)
#define SE32(val) __builtin_bswap32(val)
#define SE64(val) __builtin_bswap64(val)
#define SE128(val)                                                                                 \
    U128 {                                                                                         \
        SE64((val).hi), SE64((val).lo)                                                             \
    }
#endif

/**
 * Native endianness
 */
template <typename T>
using NativeEndian = T;

/**
 * Swapped endianness
 */
template <typename T, int size = sizeof(T)>
struct se_t;
template <typename T>
struct se_t<T, 1> {
    static inline void func(T& dst, const T src) {
        (u08&)dst = (u08&)src;
    }
};
template <typename T>
struct se_t<T, 2> {
    static inline void func(T& dst, const T src) {
        (u16&)dst = SE16((u16&)src);
    }
};
template <typename T>
struct se_t<T, 4> {
    static inline void func(T& dst, const T src) {
        (u32&)dst = SE32((u32&)src);
    }
};
template <typename T>
struct se_t<T, 8> {
    static inline void func(T& dst, const T src) {
        (u64&)dst = SE64((u64&)src);
    }
};

template <typename T>
class SwappedEndian {
    static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8,
                  "SwappedEndian only accepts types of 1, 2, 4 or 8 bytes in size");

private:
    T data;

public:
    const T& ToBE() const {
        return data;
    }

    T ToLE() const {
        T res;
        se_t<T>::func(res, data);
        return res;
    }

    void FromBE(const T& value) {
        data = value;
    }

    void FromLE(const T& value) {
        se_t<T>::func(data, value);
    }

    operator const T() const {
        return ToLE();
    }

    template <typename T1>
    operator const SwappedEndian<T1>() const {
        SwappedEndian<T1> res;
        if (sizeof(T1) < sizeof(T)) {
            res.FromBE(ToBE() >> ((sizeof(T) - sizeof(T1)) * 8));
        } else if (sizeof(T1) > sizeof(T)) {
            res.FromLE(ToLE());
        } else {
            res.FromBE(ToBE());
        }
        return res;
    }

    SwappedEndian<T>& operator=(const T& right) {
        FromLE(right);
        return *this;
    }
    SwappedEndian<T>& operator=(const SwappedEndian<T>& right) = default;

    template <typename T1>
    SwappedEndian<T>& operator+=(T1 right) {
        return *this = T(*this) + right;
    }
    template <typename T1>
    SwappedEndian<T>& operator-=(T1 right) {
        return *this = T(*this) - right;
    }
    template <typename T1>
    SwappedEndian<T>& operator*=(T1 right) {
        return *this = T(*this) * right;
    }
    template <typename T1>
    SwappedEndian<T>& operator/=(T1 right) {
        return *this = T(*this) / right;
    }
    template <typename T1>
    SwappedEndian<T>& operator%=(T1 right) {
        return *this = T(*this) % right;
    }
    template <typename T1>
    SwappedEndian<T>& operator&=(T1 right) {
        return *this = T(*this) & right;
    }
    template <typename T1>
    SwappedEndian<T>& operator|=(T1 right) {
        return *this = T(*this) | right;
    }
    template <typename T1>
    SwappedEndian<T>& operator^=(T1 right) {
        return *this = T(*this) ^ right;
    }
    template <typename T1>
    SwappedEndian<T>& operator<<=(T1 right) {
        return *this = T(*this) << right;
    }
    template <typename T1>
    SwappedEndian<T>& operator>>=(T1 right) {
        return *this = T(*this) >> right;
    }

    template <typename T1>
    SwappedEndian<T>& operator+=(const SwappedEndian<T1>& right) {
        return *this = ToLE() + right.ToLE();
    }
    template <typename T1>
    SwappedEndian<T>& operator-=(const SwappedEndian<T1>& right) {
        return *this = ToLE() - right.ToLE();
    }
    template <typename T1>
    SwappedEndian<T>& operator*=(const SwappedEndian<T1>& right) {
        return *this = ToLE() * right.ToLE();
    }
    template <typename T1>
    SwappedEndian<T>& operator/=(const SwappedEndian<T1>& right) {
        return *this = ToLE() / right.ToLE();
    }
    template <typename T1>
    SwappedEndian<T>& operator%=(const SwappedEndian<T1>& right) {
        return *this = ToLE() % right.ToLE();
    }
    template <typename T1>
    SwappedEndian<T>& operator&=(const SwappedEndian<T1>& right) {
        return *this = ToBE() & right.ToBE();
    }
    template <typename T1>
    SwappedEndian<T>& operator|=(const SwappedEndian<T1>& right) {
        return *this = ToBE() | right.ToBE();
    }
    template <typename T1>
    SwappedEndian<T>& operator^=(const SwappedEndian<T1>& right) {
        return *this = ToBE() ^ right.ToBE();
    }

    template <typename T1>
    SwappedEndian<T> operator&(const SwappedEndian<T1>& right) const {
        return SwappedEndian<T>{ToBE() & right.ToBE()};
    }
    template <typename T1>
    SwappedEndian<T> operator|(const SwappedEndian<T1>& right) const {
        return SwappedEndian<T>{ToBE() | right.ToBE()};
    }
    template <typename T1>
    SwappedEndian<T> operator^(const SwappedEndian<T1>& right) const {
        return SwappedEndian<T>{ToBE() ^ right.ToBE()};
    }

    template <typename T1>
    bool operator==(T1 right) const {
        return (T1)ToLE() == right;
    }
    template <typename T1>
    bool operator!=(T1 right) const {
        return !(*this == right);
    }
    template <typename T1>
    bool operator>(T1 right) const {
        return (T1)ToLE() > right;
    }
    template <typename T1>
    bool operator<(T1 right) const {
        return (T1)ToLE() < right;
    }
    template <typename T1>
    bool operator>=(T1 right) const {
        return (T1)ToLE() >= right;
    }
    template <typename T1>
    bool operator<=(T1 right) const {
        return (T1)ToLE() <= right;
    }

    template <typename T1>
    bool operator==(const SwappedEndian<T1>& right) const {
        return ToBE() == right.ToBE();
    }
    template <typename T1>
    bool operator!=(const SwappedEndian<T1>& right) const {
        return !(*this == right);
    }
    template <typename T1>
    bool operator>(const SwappedEndian<T1>& right) const {
        return (T1)ToLE() > right.ToLE();
    }
    template <typename T1>
    bool operator<(const SwappedEndian<T1>& right) const {
        return (T1)ToLE() < right.ToLE();
    }
    template <typename T1>
    bool operator>=(const SwappedEndian<T1>& right) const {
        return (T1)ToLE() >= right.ToLE();
    }
    template <typename T1>
    bool operator<=(const SwappedEndian<T1>& right) const {
        return (T1)ToLE() <= right.ToLE();
    }

    SwappedEndian<T> operator++(int) {
        SwappedEndian<T> res = *this;
        *this += 1;
        return res;
    }
    SwappedEndian<T> operator--(int) {
        SwappedEndian<T> res = *this;
        *this -= 1;
        return res;
    }
    SwappedEndian<T>& operator++() {
        *this += 1;
        return *this;
    }
    SwappedEndian<T>& operator--() {
        *this -= 1;
        return *this;
    }
};