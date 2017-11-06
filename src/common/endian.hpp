/*
 * Copyright (C) 2017 Larry Lopez
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#ifndef ENDIAN_HPP
#define ENDIAN_HPP

#include <common/macrohelpers.hpp>
#include <QtEndian>

namespace qkeeg {

static inline quint16 swap(quint16 x)
{
#if defined(_MSC_VER)
    return _byteswap_ushort(x);
#elif defined(__GNUC__) || defined(__clang__)
    return __builtin_bswap16(x);
#else
    return (x >> 8) | (x << 8);
#endif
}

static inline quint32 swap(quint32 x)
{
#if defined(_MSC_VER)
    return _byteswap_ulong(x);
#elif defined(__GNUC__) || defined(__clang__)
    return __builtin_bswap32(x);
#else

    return (x >> 24) |
            ((x >>  8) & 0x0000FF00) |
            ((x <<  8) & 0x00FF0000) |
            (x << 24);
#endif
}

static inline quint64 swap(quint64 x)
{
#if defined(_MSC_VER)
    return _byteswap_uint64(x);
#elif defined(__GNUC__) || defined(__clang__)
    return __builtin_bswap64(x);
#else
    return ((x & 0x00000000FFFFFFFF) << 32 | (x & 0xFFFFFFFF00000000) >> 32) |
            ((x & 0x0000FFFF0000FFFF) << 16 | (x & 0xFFFF0000FFFF0000) >> 16) |
            ((x & 0x00FF00FF00FF00FF) << 8  | (x & 0xFF00FF00FF00FF00) >> 8);
#endif
}

/// rotate left and wrap around to the right
inline quint64 rotateLeft(quint64 x, quint8 numBits)
{
#if (defined(__x86_64__) || defined(__i386) || defined(_M_IX86) || defined(_M_X64)) \
    && !defined(__clang__) && !defined(__MINGW32__)

    return _rotl64(x, numBits);
#else
    const uint64_t mask = (CHAR_BIT * sizeof(x) - 1);
    numBits &= mask;
    return (x << numBits) | (x >> ( (-numBits)&mask ));
#endif
}

/// rotate left and wrap around to the right
inline quint32 rotateLeft(quint32 x, quint8 numBits)
{
#if (defined(__x86_64__) || defined(__i386) || defined(_M_IX86) || defined(_M_X64)) && !defined(__clang__)
    return _rotl(x, numBits);
#else
    const uint32_t mask = (CHAR_BIT * sizeof(x) - 1);
    numBits &= mask;
    return (x << numBits) | (x >> ( (-numBits)&mask ));
#endif
}

/// rotate left and wrap around to the right
inline quint16 rotateLeft(quint16 x, quint8 numBits)
{
#if (defined(__x86_64__) || defined(__i386) || defined(_M_IX86) || defined(_M_X64)) && !defined(__clang__)
    #ifdef _MSC_VER
        return _rotl16(x, numBits);
    #else
        return _rotwl(x, numBits);
    #endif
#else
    const uint16_t mask = (CHAR_BIT * sizeof(x) - 1);
    numBits &= mask;
    return (x << numBits) | (x >> ( (-numBits)&mask ));
#endif
}

/// rotate left and wrap around to the right
inline quint8 rotateLeft(quint8 x, quint8 numBits)
{
#if defined(__x86_64__) || defined(__i386) || defined(_M_IX86) || defined(_M_X64) && !defined(__clang__)
    #ifdef _MSC_VER
        return _rotl8(x, numBits);
    #else
        return __rolb(x, numBits);
    #endif
#else
    const uint8_t mask = (CHAR_BIT * sizeof(x) - 1);
    numBits &= mask;
    return (x << numBits) | (x >> ( (-numBits)&mask ));
#endif
}

/// rotate right and wrap around to the left
inline quint64 rotateRight(quint64 x, quint8 numBits)
{
#if (defined(__x86_64__) || defined(__i386) || defined(_M_IX86) || defined(_M_X64)) \
    && !defined(__clang__) && !defined(__MINGW32__)

    return _rotr64(x, numBits);
#else
    const uint64_t mask = (CHAR_BIT * sizeof(x) - 1);
    numBits &= mask;
    return (x >> numBits) | (x << ((-numBits)&mask));
#endif
}

/// rotate right and wrap around to the left
inline quint32 rotateRight(quint32 x, quint8 numBits)
{
#if (defined(__x86_64__) || defined(__i386) || defined(_M_IX86) || defined(_M_X64)) && !defined(__clang__)
    return _rotr(x, numBits);
#else
    const uint32_t mask = (CHAR_BIT * sizeof(x) - 1);
    numBits &= mask;
    return (x >> numBits) | (x << ((-numBits)&mask));
#endif
}

/// rotate right and wrap around to the left
inline quint16 rotateRight(quint16 x, quint8 numBits)
{
#if (defined(__x86_64__) || defined(__i386) || defined(_M_IX86) || defined(_M_X64)) && !defined(__clang__)
    #ifdef _MSC_VER
        return _rotr16(x, numBits);
    #else
        return _rotwr(x, numBits);
    #endif
#else
    const uint16_t mask = (CHAR_BIT * sizeof(x) - 1);
    numBits &= mask;
    return (x >> numBits) | (x << ((-numBits)&mask));
#endif
}

/// rotate right and wrap around to the left
inline quint8 rotateRight(quint8 x, quint8 numBits)
{
#if (defined(__x86_64__) || defined(__i386) || defined(_M_IX86) || defined(_M_X64)) && !defined(__clang__)
    #ifdef _MSC_VER
        return _rotr8(x, numBits);
    #else
        return __rorb(x, numBits);
    #endif
#else
    const uint8_t mask = (CHAR_BIT * sizeof(x) - 1);
    numBits &= mask;
    return (x >> numBits) | (x << ((-numBits)&mask));
#endif
}

template <typename T>
inline T  big_to_native(const T& x) noexcept
{
    static_assert(std::is_integral<T>::value, "T must be any integral type.");
    return qFromBigEndian(x);
}

template <typename T>
inline T native_to_big(const T &x) noexcept
{
    static_assert(std::is_integral<T>::value, "T must be any integral type.");
    return qToBigEndian(x);
}

template <typename T>
inline T little_to_native(const T &x) noexcept
{
    static_assert(std::is_integral<T>::value, "T must be any integral type.");
    return qFromLittleEndian(x);
}

template <typename T>
inline T native_to_little(const T &x) noexcept
{
    static_assert(std::is_integral<T>::value, "T must be any integral type.");
    return qToLittleEndian(x);
}

template<typename T>
inline void big_to_native_inplace(T &x) noexcept
{
    static_assert(std::is_integral<T>::value, "T must be any integral type.");
    x = big_to_native(x);
}

template<typename T>
inline void native_to_big_inplace(T &x) noexcept
{
    static_assert(std::is_integral<T>::value, "T must be any integral type.");
    x = native_to_big(x);
}

template<typename T>
inline void little_to_native_inplace(T &x)
{
    static_assert(std::is_integral<T>::value, "T must be any integral type.");
    x = little_to_native(x);
}

template<typename T>
inline void native_to_little_inplace(T &x)
{
    static_assert(std::is_integral<T>::value, "T must be any integral type.");
    x = native_to_little(x);
}

} // namespace qkeeg

#endif // ENDIAN_HPP
