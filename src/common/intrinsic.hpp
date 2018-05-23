/*
 * Copyright (C) 2018 Larry Lopez
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
#ifndef INTRINSIC_HPP
#define INTRINSIC_HPP

#ifndef ENDIAN_NO_INTRINSICS

    #ifndef __has_builtin         // Optional of course
      #define __has_builtin(x) 0  // Compatibility with non-clang compilers
    #endif

    #if defined(_MSC_VER)
        #include <intrin.h>
        //#include <cstdlib>
        #define ENDIAN_INTRINSIC_BYTE_SWAP_2(x) _byteswap_ushort(x)
        #define ENDIAN_INTRINSIC_BYTE_SWAP_4(x) _byteswap_ulong(x)
        #define ENDIAN_INTRINSIC_BYTE_SWAP_8(x) _byteswap_uint64(x)

    //  GCC and Clang recent versions provide intrinsic byte swaps via builtins
    #elif (defined(__clang__) && __has_builtin(__builtin_bswap32) && __has_builtin(__builtin_bswap64)) \
      || (defined(__GNUC__ ) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3)))
        #define ENDIAN_INTRINSIC_MSG "__builtin_bswap16, etc."
        // prior to 4.8, gcc did not provide __builtin_bswap16 on some platforms so we emulate it
        // see http://gcc.gnu.org/bugzilla/show_bug.cgi?id=52624
        // Clang has a similar problem, but their feature test macros make it easier to detect
        #if (defined(__clang__) && __has_builtin(__builtin_bswap16)) \
          || (defined(__GNUC__) &&(__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)))
            #define ENDIAN_INTRINSIC_BYTE_SWAP_2(x) __builtin_bswap16(x)
        #else
            #define ENDIAN_INTRINSIC_BYTE_SWAP_2(x) __builtin_bswap32((x) << 16)
        #endif
        #define ENDIAN_INTRINSIC_BYTE_SWAP_4(x) __builtin_bswap32(x)
        #define ENDIAN_INTRINSIC_BYTE_SWAP_8(x) __builtin_bswap64(x)

    //  Linux systems provide the byteswap.h header, with
    #elif defined(__linux__)
        //  don't check for obsolete forms defined(linux) and defined(__linux) on the theory that
        //  compilers that predefine only these are so old that byteswap.h probably isn't present.
        #define ENDIAN_INTRINSIC_MSG "byteswap.h bswap_16, etc."
        #include <byteswap.h>
        #define ENDIAN_INTRINSIC_BYTE_SWAP_2(x) bswap_16(x)
        #define ENDIAN_INTRINSIC_BYTE_SWAP_4(x) bswap_32(x)
        #define ENDIAN_INTRINSIC_BYTE_SWAP_8(x) bswap_64(x)

    #else
        # define BOOST_ENDIAN_NO_INTRINSICS
        # define BOOST_ENDIAN_INTRINSIC_MSG "no byte swap intrinsics"

    #endif

#elif !defined(ENDIAN_INTRINSIC_MSG)
    #define ENDIAN_INTRINSIC_MSG "no byte swap intrinsics"
#endif // ENDIAN_NO_INTRINSICS

#endif // INTRINSIC_HPP
