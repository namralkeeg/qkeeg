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
#include "crc64.hpp"
#include "../../common/endian.hpp"

namespace qkeeg { namespace hashing { namespace crc {

Crc64::Crc64(const quint64 &polynomial, const quint64 &seed) : HashAlgorithm(),
    m_polynomial(polynomial), m_seed(seed)
{
    initialize();
    initializeTable();
}

void Crc64::initialize()
{
    m_hash = m_seed;
    m_hashValue.clear();
}

quint32 Crc64::hashSize()
{
    return m_hashSize;
}

void Crc64::hashCore(const void *data, const qint64 &offset, const qint64 &count)
{
    quint64 crc = ~m_hash; // same as previousCrc64 ^ 0xFFFFFFFFFFFFFFFF
    const quint8 *currentByte = reinterpret_cast<const uint8_t*>(data) + offset;
    const quint64 *current = reinterpret_cast<const uint64_t*>(currentByte);
    quint64 numBytes = count;

    // enabling optimization (at least -O2) automatically unrolls the inner for-loop
    const quint64 Unroll = 4;
    const quint64 BytesAtOnce = 8 * Unroll;

    // Process 32 (4x8) bytes each pass.
    while (numBytes >= BytesAtOnce)
    {
      for (quint64 unrolling = 0; unrolling < Unroll; unrolling++)
      {
        #if Q_BYTE_ORDER == Q_BIG_ENDIAN
          quint64 one   = *current++ ^ common::swap<quint64>(crc);
          crc = m_lookupTable[0][ one      & 0xFF] ^
                m_lookupTable[1][(one >>  8) & 0xFF] ^
                m_lookupTable[2][(one >> 16) & 0xFF] ^
                m_lookupTable[3][(one >> 24) & 0xFF] ^
                m_lookupTable[4][(one >> 32) & 0xFF] ^
                m_lookupTable[5][(one >> 40) & 0xFF] ^
                m_lookupTable[6][(one >> 48) & 0xFF] ^
                m_lookupTable[7][(one >> 56) & 0xFF];
        #else // Q_LITTLE_ENDIAN
          quint64 one   = *current++ ^ crc;
          crc = m_lookupTable[0][(one >> 56) & 0xFF] ^
                m_lookupTable[1][(one >> 48) & 0xFF] ^
                m_lookupTable[2][(one >> 40) & 0xFF] ^
                m_lookupTable[3][(one >> 32) & 0xFF] ^
                m_lookupTable[4][(one >> 24) & 0xFF] ^
                m_lookupTable[5][(one >> 16) & 0xFF] ^
                m_lookupTable[6][(one >>  8) & 0xFF] ^
                m_lookupTable[7][ one      & 0xFF];
        #endif
      }

      numBytes -= BytesAtOnce;
      currentByte += BytesAtOnce;
    }

    // remaining 1 to 63 bytes (standard algorithm)
    while (numBytes-- != 0) {
        crc = (crc >> 8) ^ m_lookupTable[0][(crc & 0xFF) ^ *currentByte++];
    }

    m_hash = ~crc;
}

QByteArray Crc64::hashFinal()
{
    QByteArray buffer(sizeof(m_hash), char(0));
    common::to_unaligned<quint64>(m_hash, buffer.data());
    return buffer;
}

} // namespace crc
} // namespace hashing
} // namespace qkeeg
