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
#ifndef CRC64_HPP
#define CRC64_HPP

#include "../hashalgorithm.hpp"
#include <array>

namespace qkeeg { namespace hashing { namespace crc {

// The CRC 64 ISO polynomial, defined in ISO 3309 and used in HDLC.
#define CRC_64_ISO_POLYNOMIAL Q_UINT64_C(0xD800000000000000)

// The ECMA polynomial, defined in ECMA 182.
#define ECMA_182_POLYNOMIAL Q_UINT64_C(0xC96C5795D7870F42)
#define JONES_POLYNOMIAL Q_UINT64_C(0xad93d23594c935a9)

// If a polynomial isn't provided, default to zlib's.
#ifndef DEFAULT_POLYNOMIAL64
    #define DEFAULT_POLYNOMIAL64 ECMA_182_POLYNOMIAL
#endif

class Crc64 : public HashAlgorithm
{
    Q_GADGET

public:
    Crc64(const quint64 &polynomial = DEFAULT_POLYNOMIAL64, const quint64 &seed = UINT32_C(0));

    // HashAlgorithm interface
public:
    virtual void initialize() override;
    virtual quint32 hashSize() override;

protected:
    virtual void hashCore(const void *data, const qint64 &offset, const qint64 &count) override;
    virtual QByteArray hashFinal() override;

private:
    static const quint32 MaxSlice = UINT32_C(8);
    static const quint32 TableEntries = UINT32_C(256);
    static const quint32 m_hashSize = std::numeric_limits<quint32>::digits;

    //! CRC64 polynomial
    quint64 m_polynomial;
    quint64 m_seed;
    quint64 m_hash;
    std::array<std::array<quint64, TableEntries>, MaxSlice> m_lookupTable;

    void initializeTable();
};

} // namespace crc
} // namespace hashing
} // namespace qkeeg

#endif // CRC64_HPP
