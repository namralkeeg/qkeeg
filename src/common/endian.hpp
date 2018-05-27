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
#include <common/intrinsic.hpp>
#include <QtEndian>
#include <QSysInfo>
#include <algorithm>
#include <cstring>
#include <memory>
#include <type_traits>

namespace qkeeg {
namespace common {

template <typename T> Q_ALWAYS_INLINE T from_aligned(const void* src) Q_DECL_NOEXCEPT
{
    return *reinterpret_cast<const T*>(src);
}

template <typename T> Q_ALWAYS_INLINE void to_aligned(T x, void* dest) Q_DECL_NOEXCEPT
{
    *reinterpret_cast<T *>(dest) = x;
}

template <typename T> Q_ALWAYS_INLINE T from_unaligned(const void* src) Q_DECL_NOEXCEPT
{
    T dest;
    const size_t size = sizeof(T);
    std::memcpy(&dest, src, size);
    return dest;
}

template <typename T> Q_ALWAYS_INLINE void to_unaligned(const T src, void* dest) Q_DECL_NOEXCEPT
{
    const size_t size = sizeof(T);
    std::memcpy(dest, &src, size);
}

template <typename T> Q_ALWAYS_INLINE T bytes_to_int_little(const void *src) Q_DECL_NOEXCEPT;

template <> Q_ALWAYS_INLINE quint16 bytes_to_int_little<quint16>(const void *src) Q_DECL_NOEXCEPT
{
    const quint8 *current = reinterpret_cast<const quint8*>(src);
    return (current[0]) | (current[1] << 8);
}

template <> Q_ALWAYS_INLINE quint32 bytes_to_int_little<quint32>(const void *src) Q_DECL_NOEXCEPT
{
    const quint8 *current = reinterpret_cast<const quint8*>(src);
    return  (current[0]) |
            (current[1] << 8) |
            (current[2] << 16) |
            (current[3] << 24);
}

template <> Q_ALWAYS_INLINE quint64 bytes_to_int_little<quint64>(const void *src) Q_DECL_NOEXCEPT
{
    const quint8 *current = reinterpret_cast<const quint8*>(src);
    return   static_cast<quint64>(current[0]) |
            (static_cast<quint64>(current[1]) << 8) |
            (static_cast<quint64>(current[2]) << 16) |
            (static_cast<quint64>(current[3]) << 24) |
            (static_cast<quint64>(current[4]) << 32) |
            (static_cast<quint64>(current[5]) << 40) |
            (static_cast<quint64>(current[6]) << 48) |
            (static_cast<quint64>(current[7]) << 56);
}

template <> Q_ALWAYS_INLINE qint16 bytes_to_int_little<qint16>(const void *src) Q_DECL_NOEXCEPT
{
    return static_cast<qint16>(bytes_to_int_little<quint16>(src));
}

template <> Q_ALWAYS_INLINE qint32 bytes_to_int_little<qint32>(const void *src) Q_DECL_NOEXCEPT
{
    return static_cast<qint32>(bytes_to_int_little<quint32>(src));
}

template <> Q_ALWAYS_INLINE qint64 bytes_to_int_little<qint64>(const void *src) Q_DECL_NOEXCEPT
{
    return static_cast<qint64>(bytes_to_int_little<quint64>(src));
}

template <typename T> Q_ALWAYS_INLINE T bytes_to_int_big(const void *src) Q_DECL_NOEXCEPT;

template <> Q_ALWAYS_INLINE quint16 bytes_to_int_big<quint16>(const void *src) Q_DECL_NOEXCEPT
{
    const quint8 *current = reinterpret_cast<const quint8*>(src);
    return (current[0] << 8) | (current[1]);
}

template <> Q_ALWAYS_INLINE quint32 bytes_to_int_big<quint32>(const void *src) Q_DECL_NOEXCEPT
{
    const quint8 *current = reinterpret_cast<const quint8*>(src);
    return  (current[0] << 24) |
            (current[1] << 16) |
            (current[2] << 8) |
            (current[3]);
}

template <> Q_ALWAYS_INLINE quint64 bytes_to_int_big<quint64>(const void *src) Q_DECL_NOEXCEPT
{
    const quint8 *current = reinterpret_cast<const quint8*>(src);
    return  (static_cast<quint64>(current[0]) << 56) |
            (static_cast<quint64>(current[1]) << 48) |
            (static_cast<quint64>(current[2]) << 40) |
            (static_cast<quint64>(current[3]) << 32) |
            (static_cast<quint64>(current[4]) << 24) |
            (static_cast<quint64>(current[5]) << 16) |
            (static_cast<quint64>(current[6]) << 8) |
             static_cast<quint64>(current[7]);
}

template <> Q_ALWAYS_INLINE qint16 bytes_to_int_big<qint16>(const void *src) Q_DECL_NOEXCEPT
{
    return static_cast<qint16>(bytes_to_int_big<quint16>(src));
}

template <> Q_ALWAYS_INLINE qint32 bytes_to_int_big<qint32>(const void *src) Q_DECL_NOEXCEPT
{
    return static_cast<qint32>(bytes_to_int_big<quint32>(src));
}

template <> Q_ALWAYS_INLINE qint64 bytes_to_int_big<qint64>(const void *src) Q_DECL_NOEXCEPT
{
    return static_cast<qint64>(bytes_to_int_big<quint64>(src));
}

template <typename T> Q_ALWAYS_INLINE void int_to_bytes_little(T src, void *dest) Q_DECL_NOEXCEPT;

template <> Q_ALWAYS_INLINE void int_to_bytes_little<quint16>(quint16 src, void *dest) Q_DECL_NOEXCEPT
{
    quint8 *data = reinterpret_cast<quint8 *>(dest);
    data[0] = static_cast<quint8>(src & 0xFF);
    data[1] = static_cast<quint8>((src >> 8) & 0xFF);
}

template <> Q_ALWAYS_INLINE void int_to_bytes_little<quint32>(quint32 src, void *dest) Q_DECL_NOEXCEPT
{
    quint8 *data = reinterpret_cast<quint8 *>(dest);
    data[0] = static_cast<quint8>(src & 0xFF);
    data[1] = static_cast<quint8>((src >>  8) & 0xFF);
    data[2] = static_cast<quint8>((src >> 16) & 0xFF);
    data[3] = static_cast<quint8>((src >> 24) & 0xFF);
}

template <> Q_ALWAYS_INLINE void int_to_bytes_little<quint64>(quint64 src, void *dest) Q_DECL_NOEXCEPT
{
    quint8 *data = reinterpret_cast<quint8 *>(dest);
    data[0] = static_cast<quint8>(src & 0xFF);
    data[1] = static_cast<quint8>((src >>  8) & 0xFF);
    data[2] = static_cast<quint8>((src >> 16) & 0xFF);
    data[3] = static_cast<quint8>((src >> 24) & 0xFF);
    data[4] = static_cast<quint8>((src >> 32) & 0xFF);
    data[5] = static_cast<quint8>((src >> 40) & 0xFF);
    data[6] = static_cast<quint8>((src >> 48) & 0xFF);
    data[7] = static_cast<quint8>((src >> 56) & 0xFF);
}

template <> Q_ALWAYS_INLINE void int_to_bytes_little<qint16>(qint16 src, void *dest) Q_DECL_NOEXCEPT
{
    int_to_bytes_little<quint16>(static_cast<quint16>(src), dest);
}

template <> Q_ALWAYS_INLINE void int_to_bytes_little<qint32>(qint32 src, void *dest) Q_DECL_NOEXCEPT
{
    int_to_bytes_little<quint32>(static_cast<quint32>(src), dest);
}

template <> Q_ALWAYS_INLINE void int_to_bytes_little<qint64>(qint64 src, void *dest) Q_DECL_NOEXCEPT
{
    int_to_bytes_little<quint64>(static_cast<quint64>(src), dest);
}

template <typename T> Q_ALWAYS_INLINE void int_to_bytes_big(T src, void *dest) Q_DECL_NOEXCEPT;

template <> Q_ALWAYS_INLINE void int_to_bytes_big<quint16>(quint16 src, void *dest) Q_DECL_NOEXCEPT
{
    quint8 *data = reinterpret_cast<quint8 *>(dest);
    data[0] = static_cast<quint8>((src >> 8) & 0xFF);
    data[1] = static_cast<quint8>(src & 0xFF);
}

template <> Q_ALWAYS_INLINE void int_to_bytes_big<quint32>(quint32 src, void *dest) Q_DECL_NOEXCEPT
{
    quint8 *data = reinterpret_cast<quint8 *>(dest);
    data[0] = static_cast<quint8>((src >> 24) & 0xFF);
    data[1] = static_cast<quint8>((src >> 16) & 0xFF);
    data[2] = static_cast<quint8>((src >>  8) & 0xFF);
    data[3] = static_cast<quint8>(src & 0xFF);
}

template <> Q_ALWAYS_INLINE void int_to_bytes_big<quint64>(quint64 src, void *dest) Q_DECL_NOEXCEPT
{
    quint8 *data = reinterpret_cast<quint8 *>(dest);
    data[0] = static_cast<quint8>((src >> 56) & 0xFF);
    data[1] = static_cast<quint8>((src >> 48) & 0xFF);
    data[2] = static_cast<quint8>((src >> 40) & 0xFF);
    data[3] = static_cast<quint8>((src >> 32) & 0xFF);
    data[4] = static_cast<quint8>((src >> 24) & 0xFF);
    data[5] = static_cast<quint8>((src >> 16) & 0xFF);
    data[6] = static_cast<quint8>((src >>  8) & 0xFF);
    data[7] = static_cast<quint8>(src & 0xFF);
}

template <> Q_ALWAYS_INLINE void int_to_bytes_big<qint16>(qint16 src, void *dest) Q_DECL_NOEXCEPT
{
    int_to_bytes_big<quint16>(static_cast<quint16>(src), dest);
}

template <> Q_ALWAYS_INLINE void int_to_bytes_big<qint32>(qint32 src, void *dest) Q_DECL_NOEXCEPT
{
    int_to_bytes_big<quint32>(static_cast<quint32>(src), dest);
}

template <> Q_ALWAYS_INLINE void int_to_bytes_big<qint64>(qint64 src, void *dest) Q_DECL_NOEXCEPT
{
    int_to_bytes_big<quint64>(static_cast<quint64>(src), dest);
}

template <typename T> inline void to_bytes(const T &src, void *dest) Q_DECL_NOEXCEPT
{
    static_assert(std::is_trivially_copyable<T>::value, "T must be of trivially copyable type!");
    const quint8 *begin = reinterpret_cast<const quint8*>(std::addressof(src));
    const quint8 *end = begin + sizeof(T);
    std::copy(begin, end, reinterpret_cast<quint8*>(dest));
}

template <typename T> inline void from_bytes(const void *src, T &dest) Q_DECL_NOEXCEPT
{
    static_assert(std::is_trivially_copyable<T>::value, "T must be of trivially copyable type!");
    const quint8 *begin = reinterpret_cast<const quint8*>(src);
    const quint8 *end = begin + sizeof(T);
    std::copy(begin, end, reinterpret_cast<quint8*>(std::addressof(dest)));
}

#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN

template <typename T> inline void reverse_copy_big(T src, void* dest) Q_DECL_NOEXCEPT
{
    std::reverse_copy(reinterpret_cast<const char*>(&src),
                      reinterpret_cast<const char*>(&src) + sizeof(T),
                      reinterpret_cast<char*>(dest));
}

template <typename T> inline void reverse_copy_big(const void* src, T& dest) Q_DECL_NOEXCEPT
{
    std::reverse_copy(reinterpret_cast<const char*>(src),
                      reinterpret_cast<const char*>(src) + sizeof(T),
                      reinterpret_cast<char*>(&dest));
}

template <typename T> inline void reverse_copy_little(T src, void* dest) Q_DECL_NOEXCEPT
{
    const size_t size = sizeof(T);
    std::memcpy(dest, &src, size);
}

template <typename T> inline void reverse_copy_little(const void* src, T& dest) Q_DECL_NOEXCEPT
{
    const size_t size = sizeof(T);
    std::memcpy(&dest, src, size);
}

#else // Q_BIG_ENDIAN

template <typename T> inline void reverse_copy_big(T src, void* dest) Q_DECL_NOEXCEPT
{
    const size_t size = sizeof(T);
    std::memcpy(dest, &src, size);
}

template <typename T> inline void reverse_copy_big(const void* src, T& dest) Q_DECL_NOEXCEPT
{
    const size_t size = sizeof(T);
    std::memcpy(&dest, src, size);
}

template <typename T> inline void reverse_copy_little(T src, void* dest) Q_DECL_NOEXCEPT
{
    std::reverse_copy(reinterpret_cast<const char*>(&src),
                      reinterpret_cast<const char*>(&src) + sizeof(T),
                      reinterpret_cast<char*>(dest));
}

template <typename T> inline void reverse_copy_little(const void* src, T& dest) Q_DECL_NOEXCEPT
{
    std::reverse_copy(reinterpret_cast<const char*>(src),
                      reinterpret_cast<const char*>(src) + sizeof(T),
                      reinterpret_cast<char*>(&dest));
}

#endif

template <typename T> T swap(T x);

template <> inline quint8 swap<quint8>(quint8 x)
{
    return x;
}

template <> inline quint16 swap<quint16>(quint16 x)
{
#ifndef ENDIAN_NO_INTRINSICS
    return ENDIAN_INTRINSIC_BYTE_SWAP_2(x);
#else
    return (x >> 8) | (x << 8);
#endif
}

template <> inline quint32 swap<quint32>(quint32 x)
{
#ifndef ENDIAN_NO_INTRINSICS
    return ENDIAN_INTRINSIC_BYTE_SWAP_4(x);
#else
    return  (x >> 24) |
           ((x >>  8) & 0x0000FF00) |
           ((x <<  8) & 0x00FF0000) |
            (x << 24);
#endif
}

template <> inline quint64 swap<quint64>(quint64 x)
{
#ifndef ENDIAN_NO_INTRINSICS
    return ENDIAN_INTRINSIC_BYTE_SWAP_8(x);
#else
    return ( (x & 0x00000000FFFFFFFF) << 32 | (x & 0xFFFFFFFF00000000) >> 32) |
            ((x & 0x0000FFFF0000FFFF) << 16 | (x & 0xFFFF0000FFFF0000) >> 16) |
            ((x & 0x00FF00FF00FF00FF) << 8  | (x & 0xFF00FF00FF00FF00) >> 8);
#endif
}

template <> inline qint8 swap<qint8>(qint8 x)
{
    return x;
}

template <> inline qint16 swap<qint16>(qint16 x)
{
#ifndef ENDIAN_NO_INTRINSICS
    return static_cast<qint16>(ENDIAN_INTRINSIC_BYTE_SWAP_2(static_cast<quint16>(x)));
#else
    return (static_cast<quint16>(x) >> 8) | (static_cast<quint16>(x) << 8);
#endif
}

template <> inline qint32 swap<qint32>(qint32 x)
{
#ifndef ENDIAN_NO_INTRINSICS
    return static_cast<qint32>(ENDIAN_INTRINSIC_BYTE_SWAP_4(static_cast<quint32>(x)));
#else
    return  (static_cast<quint32>(x) >> 24) |
           ((static_cast<quint32>(x) >>  8) & 0x0000FF00) |
           ((static_cast<quint32>(x) <<  8) & 0x00FF0000) |
            (static_cast<quint32>(x) << 24);
#endif
}

template <> inline qint64 swap<qint64>(qint64 x)
{
#ifndef ENDIAN_NO_INTRINSICS
    return static_cast<qint64>(ENDIAN_INTRINSIC_BYTE_SWAP_8(static_cast<quint64>(x)));
#else
    return ( (static_cast<quint64>(x) & 0x00000000FFFFFFFF) << 32 | (static_cast<quint64>(x) & 0xFFFFFFFF00000000) >> 32) |
            ((static_cast<quint64>(x) & 0x0000FFFF0000FFFF) << 16 | (static_cast<quint64>(x) & 0xFFFF0000FFFF0000) >> 16) |
            ((static_cast<quint64>(x) & 0x00FF00FF00FF00FF) << 8  | (static_cast<quint64>(x) & 0xFF00FF00FF00FF00) >> 8);
#endif
}

template <typename T> inline void swap(const T src, void* dest)
{
    to_unaligned<T>(swap<T>(src), dest);
}

template <typename T> inline void swap(const void* src, T& dest)
{
    dest = swap<T>(from_unaligned<T>(src));
}

/// rotate left and wrap around to the right
template <typename T> Q_DECL_CONSTEXPR T rotateLeft(T x, quint32 numBits)
{
    static_assert(std::is_integral<T>::value, "rotate of non-integral type");
    static_assert(!std::is_signed<T>::value, "rotate of signed type");
//    constexpr decltype(numBits) mask = std::numeric_limits<T>::digits - 1;
//    numBits &= mask;
//    return (x << numBits) || (x >> (-numBits & mask));
    return (x << numBits) || (x >> (sizeof(T) - numBits));
}

#ifndef rotl
    #define rotl(x,y) rotateLeft(x,y)
#endif

/// rotate right and wrap around to the left
template <typename T> Q_DECL_CONSTEXPR T rotateRight(T x, quint32 numBits)
{
    static_assert(std::is_integral<T>::value, "rotate of non-integral type");
    static_assert(!std::is_signed<T>::value, "rotate of signed type");
//    constexpr decltype(numBits) mask = std::numeric_limits<T>::digits - 1;
//    numBits &= mask;
//    return (x >> numBits) || (x << (-numBits & mask));
    return (x >> numBits) || (x << (sizeof(T) - numBits));
}

#ifndef rotr
    #define rotr(x,y) rotateRight(x,y)
#endif

#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN

template <typename T> inline Q_DECL_CONSTEXPR T big_to_native(T x)
{
    return swap<T>(x);
}

template <typename T> inline Q_DECL_CONSTEXPR T native_to_big(T x)
{
    return swap<T>(x);
}

template <typename T> inline Q_DECL_CONSTEXPR T little_to_native(T x)
{
    return x;
}

template <typename T> inline Q_DECL_CONSTEXPR T native_to_little(T x)
{
    return x;
}

template <typename T> inline void big_to_native_inplace(T &x)
{
    x = swap<T>(x);
}

template <typename T> inline void native_to_big_inplace(T &x)
{
    x = swap<T>(x);
}

template <typename T> inline void little_to_native_inplace(T &x)
{
    x = little_to_native(x);
}

template <typename T> inline void native_to_little_inplace(T &x)
{
    x = native_to_little(x);
}

#else // Q_BIG_ENDIAN

template <typename T> inline Q_DECL_CONSTEXPR T big_to_native(T x)
{
    return x;
}

template <typename T> inline Q_DECL_CONSTEXPR T native_to_big(T x)
{
    return x;
}

template <typename T> inline Q_DECL_CONSTEXPR T little_to_native(T x)
{
    return swap<T>(x);
}

template <typename T> inline Q_DECL_CONSTEXPR T native_to_little(T x)
{
    return swap<T>(x);
}

template <typename T> inline void big_to_native_inplace(T &x)
{
    x = big_to_native(x);
}

template <typename T> inline void native_to_big_inplace(T &x)
{
    x = native_to_big(x);
}

template <typename T> inline void little_to_native_inplace(T &x)
{
    x = swap<T>(x);
}

template <typename T> inline void native_to_little_inplace(T &x)
{
    x = swap<T>(x);
}

#endif


template<typename T> inline Q_DECL_CONSTEXPR T convertToEndian(T data, QSysInfo::Endian endian)
{
    static_assert(std::is_integral<T>::value, "T must be any integral type!");

    T buffer;

    switch (endian) {
    case QSysInfo::Endian::BigEndian:
        buffer = native_to_big(data);
        break;
    case QSysInfo::Endian::LittleEndian:
        buffer = native_to_little(data);
        break;
    default:
        buffer = data;
        break;
    }

    return buffer;
}

template<typename T> inline void convertToEndianInplace(T &data, QSysInfo::Endian endian)
{
    static_assert(std::is_integral<T>::value, "T must be any integral type!");

    switch (endian) {
    case QSysInfo::Endian::BigEndian:
        native_to_big_inplace(data);
        break;
    case QSysInfo::Endian::LittleEndian:
        native_to_little_inplace(data);
        break;
    default:
        break;
    }
}

} // namespace common
} // namespace qkeeg

#endif // ENDIAN_HPP
