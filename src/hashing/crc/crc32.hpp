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
#ifndef CRC32_HPP
#define CRC32_HPP

#include "../hashalgorithm.hpp"
#include <array>
#include <cstdint>

namespace qkeeg { namespace hashing { namespace crc {

// zlib's CRC32 polynomial
#define ZLIB_POLYNOMIAL UINT32_C(0xEDB88320)

// If a polynomial isn't provided, default to zlib's.
#ifndef DEFAULT_POLYNOMIAL32
    #define DEFAULT_POLYNOMIAL32 ZLIB_POLYNOMIAL
#endif

class Crc32 : public HashAlgorithm
{
    Q_GADGET

public:
    //! Constructor
    Crc32(const quint32 &polynomial = DEFAULT_POLYNOMIAL32, const quint32 &seed = UINT32_C(0));

    // HashAlgorithm interface
public:
    virtual void initialize() override;
    virtual quint32 hashSize() override;

protected:
    virtual void hashCore(const void *data, const qint64 &offset, const qint64 &count) override;
    virtual QByteArray hashFinal() override;

private:
    #ifdef CRC32_SLICING_BY_16
    static const quint32 MaxSlice = UINT32_C(16);
    #elif defined(CRC32_SLICING_BY_8)
    static const quint32 MaxSlice = UINT32_C(8);
    #elif defined(CRC32_SLICING_BY_4)
    static const quint32 MaxSlice = UINT32_C(4);
    #else
    static const quint32 MaxSlice = UINT32_C(1);
    #endif
    static const quint32 TableEntries  = UINT32_C(256);
    static const quint32 m_hashSize    = std::numeric_limits<quint32>::digits;

    //! CRC32 polynomial
    quint32 m_polynomial;
    quint32 m_seed;
    quint32 m_hash;
    std::array<std::array<quint32, TableEntries>, MaxSlice> m_lookupTable;

    void initializeTable();
};

} // namespace crc
} // namespace hashing
} // namespace qkeeg

#endif // CRC32_HPP
