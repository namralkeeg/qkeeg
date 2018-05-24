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

void Crc32::hashCore(const void *data, const qint64 &offset, const qint64 &count)
{
    quint32 crc = ~m_hash; // same as previousCrc32 ^ 0xFFFFFFFF
    const quint8 *currentByte = static_cast<const quint8*>(data) + offset;
    quint64 numBytes = count;

    while (numBytes-- != 0) {
        crc = (crc >> 8) ^ m_lookupTable[(crc & 0xFF) ^ *currentByte++];
    }

    m_hash = ~crc;
}

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

        m_lookupTable[i] = entry;
    }
}

} // namespace crc
} // namespace hashing
} // namespace qkeeg
