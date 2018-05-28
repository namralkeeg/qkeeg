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
#include "md5.hpp"
#include "../../common/endian.hpp"

namespace qkeeg { namespace hashing { namespace cryptographic {

namespace {

#ifndef ROTATELEFT
    #define ROTATELEFT(x,y) qkeeg::common::rotateLeft((x),(y))
#endif

// computations are little endian, swap data if necessary
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
    #define LITTLEENDIAN(x) qkeeg::common::swap<quint32>(x)
#else // Q_LITTLE_ENDIAN
    #define LITTLEENDIAN(x) (x)
#endif

constexpr quint32 f1(uint32_t b, uint32_t c, uint32_t d)
{
    return d ^ (b & (c ^ d)); // original: f = (b & c) | ((~b) & d);
}

constexpr quint32 f2(uint32_t b, uint32_t c, uint32_t d)
{
    return c ^ (d & (b ^ c)); // original: f = (b & d) | (c & (~d));
}

constexpr quint32 f3(uint32_t b, uint32_t c, uint32_t d)
{
    return b ^ c ^ d;
}

constexpr quint32 f4(uint32_t b, uint32_t c, uint32_t d)
{
    return c ^ (b | ~d);
}

} // anonymous namespace block

Md5::Md5()
{
    initialize();
}

Md5::~Md5()
{
    std::fill(m_buffer.begin(), m_buffer.end(), 0);
    std::fill(m_hash.begin(), m_hash.end(), 0);
}

void Md5::initialize()
{
    m_numBytes   = 0;
    m_bufferSize = 0;
    m_hashValue.clear();
    std::fill(m_buffer.begin(), m_buffer.end(), 0);

    // according to RFC 1321
    m_hash[0] = UINT32_C(0x67452301);
    m_hash[1] = UINT32_C(0xefcdab89);
    m_hash[2] = UINT32_C(0x98badcfe);
    m_hash[3] = UINT32_C(0x10325476);
}

quint32 Md5::hashSize()
{
    return m_hashSize;
}

void Md5::hashCore(const void *data, const qint64 &offset, const qint64 &count)
{
    const quint8* current = reinterpret_cast<const quint8*>(data) + offset;
    qint64 numBytes = count;

    if (m_bufferSize > 0) {
        while (numBytes > 0 && m_bufferSize < BLOCK_SIZE) {
            m_buffer[m_bufferSize++] = *current++;
            numBytes--;
        }
    }

    // full buffer
    if (m_bufferSize == BLOCK_SIZE) {
        processBlock(m_buffer.data());
        m_numBytes  += BLOCK_SIZE;
        m_bufferSize = 0;
    }

    // no more data ?
    if (numBytes == 0) {
        return;
    }

    // process full blocks
    while (numBytes >= BLOCK_SIZE) {
        processBlock(current);
        current    += BLOCK_SIZE;
        m_numBytes += BLOCK_SIZE;
        numBytes   -= BLOCK_SIZE;
    }

    // keep remaining bytes in buffer
    while (numBytes > 0) {
        m_buffer[m_bufferSize++] = *current++;
        numBytes--;
    }
}

QByteArray Md5::hashFinal()
{
    // save old hash if buffer is partially filled
    std::array<quint32, NUM_HASH_VALUES> oldHash{m_hash};

    // process remaining bytes
    processBuffer();

    QByteArray buffer(m_hashSize / 8, char(0));
    qkeeg::common::int_to_bytes_little(m_hash[0], buffer.data());
    qkeeg::common::int_to_bytes_little(m_hash[1], buffer.data()+4);
    qkeeg::common::int_to_bytes_little(m_hash[2], buffer.data()+8);
    qkeeg::common::int_to_bytes_little(m_hash[3], buffer.data()+12);

    // restore old hash
    std::copy(oldHash.begin(), oldHash.end(), m_hash.begin());

    return buffer;
}

void Md5::processBlock(const void *data)
{
    // get last hash
    quint32 a = m_hash[0];
    quint32 b = m_hash[1];
    quint32 c = m_hash[2];
    quint32 d = m_hash[3];

    // data represented as 16x 32-bit words
    const quint32* words = reinterpret_cast<const quint32*>(data);

    // first round
    quint32 word0  = LITTLEENDIAN(words[ 0]);
    a = ROTATELEFT(a + f1(b,c,d) + word0  + 0xd76aa478,  7) + b;
    quint32 word1  = LITTLEENDIAN(words[ 1]);
    d = ROTATELEFT(d + f1(a,b,c) + word1  + 0xe8c7b756, 12) + a;
    quint32 word2  = LITTLEENDIAN(words[ 2]);
    c = ROTATELEFT(c + f1(d,a,b) + word2  + 0x242070db, 17) + d;
    quint32 word3  = LITTLEENDIAN(words[ 3]);
    b = ROTATELEFT(b + f1(c,d,a) + word3  + 0xc1bdceee, 22) + c;

    quint32 word4  = LITTLEENDIAN(words[ 4]);
    a = ROTATELEFT(a + f1(b,c,d) + word4  + 0xf57c0faf,  7) + b;
    quint32 word5  = LITTLEENDIAN(words[ 5]);
    d = ROTATELEFT(d + f1(a,b,c) + word5  + 0x4787c62a, 12) + a;
    quint32 word6  = LITTLEENDIAN(words[ 6]);
    c = ROTATELEFT(c + f1(d,a,b) + word6  + 0xa8304613, 17) + d;
    quint32 word7  = LITTLEENDIAN(words[ 7]);
    b = ROTATELEFT(b + f1(c,d,a) + word7  + 0xfd469501, 22) + c;

    quint32 word8  = LITTLEENDIAN(words[ 8]);
    a = ROTATELEFT(a + f1(b,c,d) + word8  + 0x698098d8,  7) + b;
    quint32 word9  = LITTLEENDIAN(words[ 9]);
    d = ROTATELEFT(d + f1(a,b,c) + word9  + 0x8b44f7af, 12) + a;
    quint32 word10 = LITTLEENDIAN(words[10]);
    c = ROTATELEFT(c + f1(d,a,b) + word10 + 0xffff5bb1, 17) + d;
    quint32 word11 = LITTLEENDIAN(words[11]);
    b = ROTATELEFT(b + f1(c,d,a) + word11 + 0x895cd7be, 22) + c;

    quint32 word12 = LITTLEENDIAN(words[12]);
    a = ROTATELEFT(a + f1(b,c,d) + word12 + 0x6b901122,  7) + b;
    quint32 word13 = LITTLEENDIAN(words[13]);
    d = ROTATELEFT(d + f1(a,b,c) + word13 + 0xfd987193, 12) + a;
    quint32 word14 = LITTLEENDIAN(words[14]);
    c = ROTATELEFT(c + f1(d,a,b) + word14 + 0xa679438e, 17) + d;
    quint32 word15 = LITTLEENDIAN(words[15]);
    b = ROTATELEFT(b + f1(c,d,a) + word15 + 0x49b40821, 22) + c;

    // second round
    a = ROTATELEFT(a + f2(b,c,d) + word1  + 0xf61e2562,  5) + b;
    d = ROTATELEFT(d + f2(a,b,c) + word6  + 0xc040b340,  9) + a;
    c = ROTATELEFT(c + f2(d,a,b) + word11 + 0x265e5a51, 14) + d;
    b = ROTATELEFT(b + f2(c,d,a) + word0  + 0xe9b6c7aa, 20) + c;

    a = ROTATELEFT(a + f2(b,c,d) + word5  + 0xd62f105d,  5) + b;
    d = ROTATELEFT(d + f2(a,b,c) + word10 + 0x02441453,  9) + a;
    c = ROTATELEFT(c + f2(d,a,b) + word15 + 0xd8a1e681, 14) + d;
    b = ROTATELEFT(b + f2(c,d,a) + word4  + 0xe7d3fbc8, 20) + c;

    a = ROTATELEFT(a + f2(b,c,d) + word9  + 0x21e1cde6,  5) + b;
    d = ROTATELEFT(d + f2(a,b,c) + word14 + 0xc33707d6,  9) + a;
    c = ROTATELEFT(c + f2(d,a,b) + word3  + 0xf4d50d87, 14) + d;
    b = ROTATELEFT(b + f2(c,d,a) + word8  + 0x455a14ed, 20) + c;

    a = ROTATELEFT(a + f2(b,c,d) + word13 + 0xa9e3e905,  5) + b;
    d = ROTATELEFT(d + f2(a,b,c) + word2  + 0xfcefa3f8,  9) + a;
    c = ROTATELEFT(c + f2(d,a,b) + word7  + 0x676f02d9, 14) + d;
    b = ROTATELEFT(b + f2(c,d,a) + word12 + 0x8d2a4c8a, 20) + c;

    // third round
    a = ROTATELEFT(a + f3(b,c,d) + word5  + 0xfffa3942,  4) + b;
    d = ROTATELEFT(d + f3(a,b,c) + word8  + 0x8771f681, 11) + a;
    c = ROTATELEFT(c + f3(d,a,b) + word11 + 0x6d9d6122, 16) + d;
    b = ROTATELEFT(b + f3(c,d,a) + word14 + 0xfde5380c, 23) + c;

    a = ROTATELEFT(a + f3(b,c,d) + word1  + 0xa4beea44,  4) + b;
    d = ROTATELEFT(d + f3(a,b,c) + word4  + 0x4bdecfa9, 11) + a;
    c = ROTATELEFT(c + f3(d,a,b) + word7  + 0xf6bb4b60, 16) + d;
    b = ROTATELEFT(b + f3(c,d,a) + word10 + 0xbebfbc70, 23) + c;

    a = ROTATELEFT(a + f3(b,c,d) + word13 + 0x289b7ec6,  4) + b;
    d = ROTATELEFT(d + f3(a,b,c) + word0  + 0xeaa127fa, 11) + a;
    c = ROTATELEFT(c + f3(d,a,b) + word3  + 0xd4ef3085, 16) + d;
    b = ROTATELEFT(b + f3(c,d,a) + word6  + 0x04881d05, 23) + c;

    a = ROTATELEFT(a + f3(b,c,d) + word9  + 0xd9d4d039,  4) + b;
    d = ROTATELEFT(d + f3(a,b,c) + word12 + 0xe6db99e5, 11) + a;
    c = ROTATELEFT(c + f3(d,a,b) + word15 + 0x1fa27cf8, 16) + d;
    b = ROTATELEFT(b + f3(c,d,a) + word2  + 0xc4ac5665, 23) + c;

    // fourth round
    a = ROTATELEFT(a + f4(b,c,d) + word0  + 0xf4292244,  6) + b;
    d = ROTATELEFT(d + f4(a,b,c) + word7  + 0x432aff97, 10) + a;
    c = ROTATELEFT(c + f4(d,a,b) + word14 + 0xab9423a7, 15) + d;
    b = ROTATELEFT(b + f4(c,d,a) + word5  + 0xfc93a039, 21) + c;

    a = ROTATELEFT(a + f4(b,c,d) + word12 + 0x655b59c3,  6) + b;
    d = ROTATELEFT(d + f4(a,b,c) + word3  + 0x8f0ccc92, 10) + a;
    c = ROTATELEFT(c + f4(d,a,b) + word10 + 0xffeff47d, 15) + d;
    b = ROTATELEFT(b + f4(c,d,a) + word1  + 0x85845dd1, 21) + c;

    a = ROTATELEFT(a + f4(b,c,d) + word8  + 0x6fa87e4f,  6) + b;
    d = ROTATELEFT(d + f4(a,b,c) + word15 + 0xfe2ce6e0, 10) + a;
    c = ROTATELEFT(c + f4(d,a,b) + word6  + 0xa3014314, 15) + d;
    b = ROTATELEFT(b + f4(c,d,a) + word13 + 0x4e0811a1, 21) + c;

    a = ROTATELEFT(a + f4(b,c,d) + word4  + 0xf7537e82,  6) + b;
    d = ROTATELEFT(d + f4(a,b,c) + word11 + 0xbd3af235, 10) + a;
    c = ROTATELEFT(c + f4(d,a,b) + word2  + 0x2ad7d2bb, 15) + d;
    b = ROTATELEFT(b + f4(c,d,a) + word9  + 0xeb86d391, 21) + c;

    // update hash
    m_hash[0] += a;
    m_hash[1] += b;
    m_hash[2] += c;
    m_hash[3] += d;
}

void Md5::processBuffer()
{
    // the input bytes are considered as bits strings, where the first bit is the most significant bit of the byte

    // - append "1" bit to message
    // - append "0" bits until message length in bit mod 512 is 448
    // - append length as 64 bit integer

    // number of bits
    qint64 paddedLength = m_bufferSize * 8;

    // plus one bit set to 1 (always appended)
    paddedLength++;

    // number of bits must be (numBits % 512) = 448
    quint64 lower11Bits = paddedLength & 511;
    if (lower11Bits <= 448) {
        paddedLength +=       448 - lower11Bits;
    }
    else {
        paddedLength += 512 + 448 - lower11Bits;
    }

    // convert from bits to bytes
    paddedLength /= 8;

    // only needed if additional data flows over into a second block
    std::array<quint8, BLOCK_SIZE> extra;

    // append a "1" bit, 128 => binary 10000000
    if (m_bufferSize < BLOCK_SIZE) {
        m_buffer[m_bufferSize] = 128;
    }
    else {
        extra[0] = 128;
    }

    qint64 i;
    for (i = m_bufferSize + 1; i < BLOCK_SIZE; ++i) {
        m_buffer[i] = 0;
    }
    for (; i < paddedLength; ++i) {
        extra[i - BLOCK_SIZE] = 0;
    }

    // add message length in bits as 64 bit number
    quint64 msgBits = 8 * (m_numBytes + m_bufferSize);

    // find right position
    quint8* addLength;
    if (paddedLength < BLOCK_SIZE) {
        addLength = m_buffer.data() + paddedLength;
    }
    else {
        addLength = extra.data() + paddedLength - BLOCK_SIZE;
    }

    // must be little endian
    *addLength++ = msgBits & 0xFF; msgBits >>= 8;
    *addLength++ = msgBits & 0xFF; msgBits >>= 8;
    *addLength++ = msgBits & 0xFF; msgBits >>= 8;
    *addLength++ = msgBits & 0xFF; msgBits >>= 8;
    *addLength++ = msgBits & 0xFF; msgBits >>= 8;
    *addLength++ = msgBits & 0xFF; msgBits >>= 8;
    *addLength++ = msgBits & 0xFF; msgBits >>= 8;
    *addLength++ = msgBits & 0xFF;

    // process blocks
    processBlock(m_buffer.data());

    // flowed over into a second block ?
    if (paddedLength > BLOCK_SIZE) {
        processBlock(extra.data());
    }
}

} // namespace cryptographic
} // namespace hashing
} // namespace qkeeg
