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
#include "crc32.hpp"
#include "../../common/endian.hpp"

namespace qkeeg { namespace hashing { namespace crc {

Crc32::Crc32(const quint32 &polynomial, const quint32 &seed) : HashAlgorithm(),
    m_polynomial(polynomial), m_seed(seed)
{
    initialize();
    initializeTable();
}

void Crc32::initialize()
{
    m_hash = m_seed;
    m_hashValue.clear();
}

quint32 Crc32::hashSize()
{
    return m_hashSize;
}

#ifdef CRC32_SLICING_BY_16

void Crc32::hashCore(const void *data, const qint64 &offset, const qint64 &count)
{
    quint32 crc = ~m_hash; // same as previousCrc32 ^ 0xFFFFFFFF
    const quint8 *currentByte = reinterpret_cast<const quint8*>(data) + offset;
    const quint32 *current = reinterpret_cast<const quint32*>(currentByte);
    quint64 numBytes = count;

    // enabling optimization (at least -O2) automatically unrolls the inner for-loop
    const quint64 Unroll = 4;
    const quint64 BytesAtOnce = 16 * Unroll;

    // Process 64 bytes each pass.
    while (numBytes >= BytesAtOnce)
    {
      for (quint64 unrolling = 0; unrolling < Unroll; unrolling++)
      {
        #if Q_BYTE_ORDER == Q_BIG_ENDIAN
          quint32 one   = *current++ ^ common::swap<quint32>(crc);
          quint32 two   = *current++;
          quint32 three = *current++;
          quint32 four  = *current++;
          crc  =  m_lookupTable[ 0][ four         & 0xFF] ^
                  m_lookupTable[ 1][(four  >>  8) & 0xFF] ^
                  m_lookupTable[ 2][(four  >> 16) & 0xFF] ^
                  m_lookupTable[ 3][(four  >> 24) & 0xFF] ^
                  m_lookupTable[ 4][ three        & 0xFF] ^
                  m_lookupTable[ 5][(three >>  8) & 0xFF] ^
                  m_lookupTable[ 6][(three >> 16) & 0xFF] ^
                  m_lookupTable[ 7][(three >> 24) & 0xFF] ^
                  m_lookupTable[ 8][ two          & 0xFF] ^
                  m_lookupTable[ 9][(two   >>  8) & 0xFF] ^
                  m_lookupTable[10][(two   >> 16) & 0xFF] ^
                  m_lookupTable[11][(two   >> 24) & 0xFF] ^
                  m_lookupTable[12][ one          & 0xFF] ^
                  m_lookupTable[13][(one   >>  8) & 0xFF] ^
                  m_lookupTable[14][(one   >> 16) & 0xFF] ^
                  m_lookupTable[15][(one   >> 24) & 0xFF];
        #else // Q_LITTLE_ENDIAN
          quint32 one   = *current++ ^ crc;
          quint32 two   = *current++;
          quint32 three = *current++;
          quint32 four  = *current++;
          crc  =  m_lookupTable[ 0][(four  >> 24) & 0xFF] ^
                  m_lookupTable[ 1][(four  >> 16) & 0xFF] ^
                  m_lookupTable[ 2][(four  >>  8) & 0xFF] ^
                  m_lookupTable[ 3][ four         & 0xFF] ^
                  m_lookupTable[ 4][(three >> 24) & 0xFF] ^
                  m_lookupTable[ 5][(three >> 16) & 0xFF] ^
                  m_lookupTable[ 6][(three >>  8) & 0xFF] ^
                  m_lookupTable[ 7][ three        & 0xFF] ^
                  m_lookupTable[ 8][(two   >> 24) & 0xFF] ^
                  m_lookupTable[ 9][(two   >> 16) & 0xFF] ^
                  m_lookupTable[10][(two   >>  8) & 0xFF] ^
                  m_lookupTable[11][ two          & 0xFF] ^
                  m_lookupTable[12][(one   >> 24) & 0xFF] ^
                  m_lookupTable[13][(one   >> 16) & 0xFF] ^
                  m_lookupTable[14][(one   >>  8) & 0xFF] ^
                  m_lookupTable[15][ one          & 0xFF];
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

#elif defined(CRC32_SLICING_BY_8)

void Crc32::hashCore(const void *data, const qint64 &offset, const qint64 &count)
{
    quint32 crc = ~m_hash; // same as previousCrc32 ^ 0xFFFFFFFF
    const quint8 *currentByte = reinterpret_cast<const quint8*>(data) + offset;
    const quint32 *current = reinterpret_cast<const quint32*>(currentByte);
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
          quint32 one   = *current++ ^ common::swap<quint32>(crc);
          quint32 two   = *current++;
          crc = m_lookupTable[0][ two      & 0xFF] ^
                m_lookupTable[1][(two>> 8) & 0xFF] ^
                m_lookupTable[2][(two>>16) & 0xFF] ^
                m_lookupTable[3][(two>>24) & 0xFF] ^
                m_lookupTable[4][ one      & 0xFF] ^
                m_lookupTable[5][(one>> 8) & 0xFF] ^
                m_lookupTable[6][(one>>16) & 0xFF] ^
                m_lookupTable[7][(one>>24) & 0xFF];
        #else // Q_LITTLE_ENDIAN
          quint32 one   = *current++ ^ crc;
          quint32 two   = *current++;
          crc = m_lookupTable[0][(two>>24) & 0xFF] ^
                m_lookupTable[1][(two>>16) & 0xFF] ^
                m_lookupTable[2][(two>> 8) & 0xFF] ^
                m_lookupTable[3][ two      & 0xFF] ^
                m_lookupTable[4][(one>>24) & 0xFF] ^
                m_lookupTable[5][(one>>16) & 0xFF] ^
                m_lookupTable[6][(one>> 8) & 0xFF] ^
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

#elif defined(CRC32_SLICING_BY_4)

void Crc32::hashCore(const void *data, const qint64 &offset, const qint64 &count)
{
    quint32 crc = ~m_hash; // same as previousCrc32 ^ 0xFFFFFFFF
    const quint8 *currentByte = reinterpret_cast<const quint8*>(data) + offset;
    const quint32 *current = reinterpret_cast<const quint32*>(currentByte);
    quint64 numBytes = count;

    // enabling optimization (at least -O2) automatically unrolls the inner for-loop
    const quint64 Unroll = 4;
    const quint64 BytesAtOnce = 4 * Unroll;

    // Process 16 (4x4) bytes each pass.
    while (numBytes >= BytesAtOnce)
    {
      for (quint64 unrolling = 0; unrolling < Unroll; unrolling++)
      {
        #if Q_BYTE_ORDER == Q_BIG_ENDIAN
          quint32 one   = *current++ ^ common::swap<quint32>(crc);
          crc = m_lookupTable[0][ one      & 0xFF] ^
                m_lookupTable[1][(one>> 8) & 0xFF] ^
                m_lookupTable[2][(one>>16) & 0xFF] ^
                m_lookupTable[3][(one>>24) & 0xFF];
        #else // Q_LITTLE_ENDIAN
          quint32 one   = *current++ ^ crc;
          crc = m_lookupTable[0][(one>>24) & 0xFF] ^
                m_lookupTable[1][(one>>16) & 0xFF] ^
                m_lookupTable[2][(one>> 8) & 0xFF] ^
                m_lookupTable[3][ one      & 0xFF];
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

#else // Default 1 byte table lookup.

void Crc32::hashCore(const void *data, const qint64 &offset, const qint64 &count)
{
    quint32 crc = ~m_hash; // same as previousCrc32 ^ 0xFFFFFFFF
    const quint8 *currentByte = reinterpret_cast<const quint8*>(data) + offset;
    quint64 numBytes = count;

    while (numBytes-- != 0) {
        crc = (crc >> 8) ^ m_lookupTable[0][(crc & 0xFF) ^ *currentByte++];
    }

    m_hash = ~crc;
}

#endif

QByteArray Crc32::hashFinal()
{
    QByteArray buffer(sizeof(m_hash), char(0));
    common::to_unaligned<quint32>(m_hash, buffer.data());
    return buffer;
}

void Crc32::initializeTable()
{
    quint32 entry;
    for (quint32 i = 0; i < TableEntries; ++i) {
        entry = i;
        for (auto j = 0; j < 8; ++j) {
            entry = (entry >> 1) ^ ((entry & 1) * m_polynomial);
        }

        m_lookupTable[0][i] = entry;
    }

    if (MaxSlice > 1) {
        for (quint32 i = 0; i < TableEntries; ++i)
        {
            for (quint32 slice = 1; slice < MaxSlice; ++slice)
            {
                m_lookupTable[slice][i] =
                        (m_lookupTable[slice - 1][i] >> 8) ^ m_lookupTable[0][m_lookupTable[slice - 1][i] & 0xFF];
            }
        }
    }
}

} // namespace crc
} // namespace hashing
} // namespace qkeeg
