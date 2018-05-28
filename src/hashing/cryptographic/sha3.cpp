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
#include "sha3.hpp"
#include "../../common/endian.hpp"
#include "../../common/enums.hpp"
#include <algorithm>

namespace qkeeg { namespace hashing { namespace cryptographic {

namespace
{

/// rotate left and wrap around to the right
#ifdef ROTATELEFT
    #undef ROTATELEFT
#endif
#define ROTATELEFT(x,y) qkeeg::common::rotateLeft((x),(y))

#if defined(Q_BYTE_ORDER) && (Q_BYTE_ORDER == Q_BIG_ENDIAN)
    #define LITTLEENDIAN(x) qkeeg::common::swap<quint64>(x)
#else // Q_LITTLE_ENDIAN
    #define LITTLEENDIAN(x) (x)
#endif


/// return x % 5 for 0 <= x <= 9
constexpr quint32 mod5(quint32 x)
{
    if (x < 5)
        return x;

    return x - 5;
}

static const quint32 Rounds = 24;
constexpr std::array<quint64, Rounds> XorMasks =
{
    UINT64_C(0x0000000000000001), UINT64_C(0x0000000000008082), UINT64_C(0x800000000000808a),
    UINT64_C(0x8000000080008000), UINT64_C(0x000000000000808b), UINT64_C(0x0000000080000001),
    UINT64_C(0x8000000080008081), UINT64_C(0x8000000000008009), UINT64_C(0x000000000000008a),
    UINT64_C(0x0000000000000088), UINT64_C(0x0000000080008009), UINT64_C(0x000000008000000a),
    UINT64_C(0x000000008000808b), UINT64_C(0x800000000000008b), UINT64_C(0x8000000000008089),
    UINT64_C(0x8000000000008003), UINT64_C(0x8000000000008002), UINT64_C(0x8000000000000080),
    UINT64_C(0x000000000000800a), UINT64_C(0x800000008000000a), UINT64_C(0x8000000080008081),
    UINT64_C(0x8000000000008080), UINT64_C(0x0000000080000001), UINT64_C(0x8000000080008008)
};

} // anonymous namespace

Sha3::Sha3(Bits bits) : HashAlgorithm(), m_bits(bits)
{
    m_blockSize = 200 - 2 * (enumToIntegral(bits) / 8);
    initialize();
}

Sha3::~Sha3()
{
    std::fill(m_buffer.begin(), m_buffer.end(), 0);
    std::fill(m_hash.begin(), m_hash.end(), 0);
}

void Sha3::initialize()
{
    std::fill(m_hash.begin(), m_hash.end(), UINT64_C(0));
    m_hashValue.clear();
    m_numBytes   = 0;
    m_bufferSize = 0;
}

quint32 Sha3::hashSize()
{
    return static_cast<quint32>(enumToIntegral(m_bits));
}

void Sha3::hashCore(const void *data, const qint64 &offset, const qint64 &count)
{
    const quint8 *current = static_cast<const quint8*>(data) + offset;
    quint64 numBytes = count;

    // copy data to buffer
    if (m_bufferSize > 0) {
        while ((numBytes > 0) && (m_bufferSize < m_blockSize)) {
            m_buffer[m_bufferSize++] = *current++;
            numBytes--;
        }
    }

    // full buffer
    if (m_bufferSize == m_blockSize) {
        processBlock(m_buffer.data());
        m_numBytes  += m_blockSize;
        m_bufferSize = 0;
    }

    // no more data ?
    if (numBytes == 0) {
        return;
    }

    // process full blocks
    while (numBytes >= m_blockSize) {
        processBlock(current);
        current    += m_blockSize;
        m_numBytes += m_blockSize;
        numBytes   -= m_blockSize;
    }

    // keep remaining bytes in buffer
    while (numBytes > 0) {
        m_buffer[m_bufferSize++] = *current++;
        numBytes--;
    }
}

QByteArray Sha3::hashFinal()
{
    // process remaining bytes
    processBuffer();

    // number of significant elements in hash (uint64_t)
    quint32 hashLength = hashSize() / 64;
    QByteArray buffer;
    buffer.reserve(hashSize() / sizeof(quint8));

    for (quint32 i = 0; i < hashLength; ++i)
    {
        for (quint32 j = 0; j < 8; ++j)
        {
            buffer.append(static_cast<char>((m_hash[i] >> (8 * j)) & 0xFF));
        }
    }

    // SHA3-224's last entry in m_hash provides only 32 bits instead of 64 bits
    quint32 remainder = hashSize() - hashLength * 64;
    quint32 processed = 0;
    while (processed < remainder)
    {
        buffer.append(static_cast<char>(m_hash[hashLength] >> processed));
        processed += 8;
    }

    return buffer;
}

void Sha3::processBlock(const void *data)
{
    const quint64* data64 = static_cast<const quint64*>(data);

    // mix data into state
    for (quint32 i = 0; i < m_blockSize / 8; i++) {
        m_hash[i] ^= LITTLEENDIAN(data64[i]);
    }

    // re-compute state
    for (quint32 round = 0; round < Rounds; round++) {
        // Theta
        std::array<quint64, 5> coefficients;
        for (quint32 i = 0; i < 5; i++) {
            coefficients[i] = m_hash[i] ^ m_hash[i + 5] ^ m_hash[i + 10] ^ m_hash[i + 15] ^ m_hash[i + 20];
        }

        for (quint32 i = 0; i < 5; i++) {
            quint64 one = coefficients[mod5(i + 4)] ^ ROTATELEFT(coefficients[mod5(i + 1)], 1);
            m_hash[i     ] ^= one;
            m_hash[i +  5] ^= one;
            m_hash[i + 10] ^= one;
            m_hash[i + 15] ^= one;
            m_hash[i + 20] ^= one;
        }

        // temporary
        quint64 one;

        // Rho Pi
        quint64 last = m_hash[1];
        one = m_hash[10]; m_hash[10] = ROTATELEFT(last,  1); last = one;
        one = m_hash[ 7]; m_hash[ 7] = ROTATELEFT(last,  3); last = one;
        one = m_hash[11]; m_hash[11] = ROTATELEFT(last,  6); last = one;
        one = m_hash[17]; m_hash[17] = ROTATELEFT(last, 10); last = one;
        one = m_hash[18]; m_hash[18] = ROTATELEFT(last, 15); last = one;
        one = m_hash[ 3]; m_hash[ 3] = ROTATELEFT(last, 21); last = one;
        one = m_hash[ 5]; m_hash[ 5] = ROTATELEFT(last, 28); last = one;
        one = m_hash[16]; m_hash[16] = ROTATELEFT(last, 36); last = one;
        one = m_hash[ 8]; m_hash[ 8] = ROTATELEFT(last, 45); last = one;
        one = m_hash[21]; m_hash[21] = ROTATELEFT(last, 55); last = one;
        one = m_hash[24]; m_hash[24] = ROTATELEFT(last,  2); last = one;
        one = m_hash[ 4]; m_hash[ 4] = ROTATELEFT(last, 14); last = one;
        one = m_hash[15]; m_hash[15] = ROTATELEFT(last, 27); last = one;
        one = m_hash[23]; m_hash[23] = ROTATELEFT(last, 41); last = one;
        one = m_hash[19]; m_hash[19] = ROTATELEFT(last, 56); last = one;
        one = m_hash[13]; m_hash[13] = ROTATELEFT(last,  8); last = one;
        one = m_hash[12]; m_hash[12] = ROTATELEFT(last, 25); last = one;
        one = m_hash[ 2]; m_hash[ 2] = ROTATELEFT(last, 43); last = one;
        one = m_hash[20]; m_hash[20] = ROTATELEFT(last, 62); last = one;
        one = m_hash[14]; m_hash[14] = ROTATELEFT(last, 18); last = one;
        one = m_hash[22]; m_hash[22] = ROTATELEFT(last, 39); last = one;
        one = m_hash[ 9]; m_hash[ 9] = ROTATELEFT(last, 61); last = one;
        one = m_hash[ 6]; m_hash[ 6] = ROTATELEFT(last, 20); last = one;
                          m_hash[ 1] = ROTATELEFT(last, 44);

        // Chi
        for (quint32 j = 0; j < 25; j += 5) {
            // temporaries
            quint64 one = m_hash[j];
            quint64 two = m_hash[j + 1];

            m_hash[j]     ^= m_hash[j + 2] & ~two;
            m_hash[j + 1] ^= m_hash[j + 3] & ~m_hash[j + 2];
            m_hash[j + 2] ^= m_hash[j + 4] & ~m_hash[j + 3];
            m_hash[j + 3] ^=      one      & ~m_hash[j + 4];
            m_hash[j + 4] ^=      two      & ~one;
        }

        // Iota
        m_hash[0] ^= XorMasks[round];
    }
}

void Sha3::processBuffer()
{
    // add padding
    quint64 offset = m_bufferSize;

    // add a "1" byte
    m_buffer[offset++] = 0x06;

    // fill with zeros
    while (offset < m_blockSize) {
        m_buffer[offset++] = 0;
    }

    // and add a single set bit
    m_buffer[offset - 1] |= 0x80;

    processBlock(m_buffer.data());
}

} // namespace cryptographic
} // namespace hashing
} // namespace qkeeg
