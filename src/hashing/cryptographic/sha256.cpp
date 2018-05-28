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
#include "sha256.hpp"
#include "../../common/endian.hpp"

namespace qkeeg { namespace hashing { namespace cryptographic {

namespace
{

#ifdef ROTATERIGHT
    #undef ROTATERIGHT
#else
    #define ROTATERIGHT(x,y) qkeeg::common::rotateRight((x),(y))
#endif

// mix functions for processBlock()
constexpr quint32 f1(quint32 e, quint32 f, quint32 g)
{
    quint32 term1 = ROTATERIGHT(e, 6) ^ ROTATERIGHT(e, 11) ^ ROTATERIGHT(e, 25);
    quint32 term2 = (e & f) ^ (~e & g); //(g ^ (e & (f ^ g)))
    return term1 + term2;
}

constexpr quint32 f2(quint32 a, quint32 b, quint32 c)
{
    quint32 term1 = ROTATERIGHT(a, 2) ^ ROTATERIGHT(a, 13) ^ ROTATERIGHT(a, 22);
    quint32 term2 = ((a | b) & c) | (a & b); //(a & (b ^ c)) ^ (b & c);
    return term1 + term2;
}

} // anonymous namespace

Sha256::Sha256() : HashAlgorithm()
{
    initialize();
}

Sha256::~Sha256()
{
    std::fill(m_buffer.begin(), m_buffer.end(), 0);
    std::fill(m_hash.begin(), m_hash.end(), 0);
}

void Sha256::initialize()
{
    m_hashValue.clear();
    m_numBytes   = 0;
    m_bufferSize = 0;

    // according to RFC 1321
    m_hash[0] = UINT32_C(0x6a09e667);
    m_hash[1] = UINT32_C(0xbb67ae85);
    m_hash[2] = UINT32_C(0x3c6ef372);
    m_hash[3] = UINT32_C(0xa54ff53a);
    m_hash[4] = UINT32_C(0x510e527f);
    m_hash[5] = UINT32_C(0x9b05688c);
    m_hash[6] = UINT32_C(0x1f83d9ab);
    m_hash[7] = UINT32_C(0x5be0cd19);
}

quint32 Sha256::hashSize()
{
    return m_hashSize;
}

void Sha256::hashCore(const void *data, const qint64 &offset, const qint64 &count)
{
    const quint8* current = reinterpret_cast<const uint8_t*>(data) + offset;
    qint64 numBytes = count;

    if (m_bufferSize > 0) {
        while ((numBytes > 0) && (m_bufferSize < BLOCK_SIZE)) {
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

QByteArray Sha256::hashFinal()
{
    // save old hash if buffer is partially filled
    std::array<quint32, NUM_HASH_VALUES> oldHash{m_hash};

    // process remaining bytes
    processBuffer();

    // TODO: Validate code to transform m_hash to a QByteArray
    QByteArray buffer(m_hashBytes, char(0));
    for (quint32 i = 0; i < m_hash.size(); ++i) {
        qkeeg::common::int_to_bytes_big<quint32>(m_hash[i], buffer.data() + i * sizeof(quint32));
    }

    // restore old hash
    std::copy(oldHash.begin(), oldHash.end(), m_hash.begin());

    return buffer;
}

void Sha256::processBlock(const void *data)
{
    // get last hash
    quint32 a = m_hash[0];
    quint32 b = m_hash[1];
    quint32 c = m_hash[2];
    quint32 d = m_hash[3];
    quint32 e = m_hash[4];
    quint32 f = m_hash[5];
    quint32 g = m_hash[6];
    quint32 h = m_hash[7];

    // data represented as 16x 32-bit words
    const quint32 *input = reinterpret_cast<const quint32*>(data);

    std::array<quint32, 64> words;
#if defined(Q_BYTE_ORDER) && (Q_BYTE_ORDER == Q_BIG_ENDIAN)
    std::copy(input, input + 16, words.begin());
#else // Q_LITTLE_ENDIAN
    // convert to big endian
    std::transform(input, input + 16, words.begin(),
                   [](const quint32 &b) -> quint32 { return qkeeg::common::swap<quint32>(b); });
#endif

    quint32 x,y; // temporaries

    // first round
    x = h + f1(e,f,g) + 0x428a2f98 + words[ 0]; y = f2(a,b,c); d += x; h = x + y;
    x = g + f1(d,e,f) + 0x71374491 + words[ 1]; y = f2(h,a,b); c += x; g = x + y;
    x = f + f1(c,d,e) + 0xb5c0fbcf + words[ 2]; y = f2(g,h,a); b += x; f = x + y;
    x = e + f1(b,c,d) + 0xe9b5dba5 + words[ 3]; y = f2(f,g,h); a += x; e = x + y;
    x = d + f1(a,b,c) + 0x3956c25b + words[ 4]; y = f2(e,f,g); h += x; d = x + y;
    x = c + f1(h,a,b) + 0x59f111f1 + words[ 5]; y = f2(d,e,f); g += x; c = x + y;
    x = b + f1(g,h,a) + 0x923f82a4 + words[ 6]; y = f2(c,d,e); f += x; b = x + y;
    x = a + f1(f,g,h) + 0xab1c5ed5 + words[ 7]; y = f2(b,c,d); e += x; a = x + y;

    // secound round
    x = h + f1(e,f,g) + 0xd807aa98 + words[ 8]; y = f2(a,b,c); d += x; h = x + y;
    x = g + f1(d,e,f) + 0x12835b01 + words[ 9]; y = f2(h,a,b); c += x; g = x + y;
    x = f + f1(c,d,e) + 0x243185be + words[10]; y = f2(g,h,a); b += x; f = x + y;
    x = e + f1(b,c,d) + 0x550c7dc3 + words[11]; y = f2(f,g,h); a += x; e = x + y;
    x = d + f1(a,b,c) + 0x72be5d74 + words[12]; y = f2(e,f,g); h += x; d = x + y;
    x = c + f1(h,a,b) + 0x80deb1fe + words[13]; y = f2(d,e,f); g += x; c = x + y;
    x = b + f1(g,h,a) + 0x9bdc06a7 + words[14]; y = f2(c,d,e); f += x; b = x + y;
    x = a + f1(f,g,h) + 0xc19bf174 + words[15]; y = f2(b,c,d); e += x; a = x + y;

    int i = 16;
    // extend to 24 words
    for (; i < 24; i++) {
        words[i] = words[i-16] +
                (ROTATERIGHT(words[i-15],  7) ^ ROTATERIGHT(words[i-15], 18) ^ (words[i-15] >>  3)) +
                words[i-7] +
                (ROTATERIGHT(words[i- 2], 17) ^ ROTATERIGHT(words[i- 2], 19) ^ (words[i- 2] >> 10));
    }

    // third round
    x = h + f1(e,f,g) + 0xe49b69c1 + words[16]; y = f2(a,b,c); d += x; h = x + y;
    x = g + f1(d,e,f) + 0xefbe4786 + words[17]; y = f2(h,a,b); c += x; g = x + y;
    x = f + f1(c,d,e) + 0x0fc19dc6 + words[18]; y = f2(g,h,a); b += x; f = x + y;
    x = e + f1(b,c,d) + 0x240ca1cc + words[19]; y = f2(f,g,h); a += x; e = x + y;
    x = d + f1(a,b,c) + 0x2de92c6f + words[20]; y = f2(e,f,g); h += x; d = x + y;
    x = c + f1(h,a,b) + 0x4a7484aa + words[21]; y = f2(d,e,f); g += x; c = x + y;
    x = b + f1(g,h,a) + 0x5cb0a9dc + words[22]; y = f2(c,d,e); f += x; b = x + y;
    x = a + f1(f,g,h) + 0x76f988da + words[23]; y = f2(b,c,d); e += x; a = x + y;

    // extend to 32 words
    for (; i < 32; i++) {
        words[i] = words[i-16] +
                (ROTATERIGHT(words[i-15],  7) ^ ROTATERIGHT(words[i-15], 18) ^ (words[i-15] >>  3)) +
                words[i-7] +
                (ROTATERIGHT(words[i- 2], 17) ^ ROTATERIGHT(words[i- 2], 19) ^ (words[i- 2] >> 10));
    }

    // fourth round
    x = h + f1(e,f,g) + 0x983e5152 + words[24]; y = f2(a,b,c); d += x; h = x + y;
    x = g + f1(d,e,f) + 0xa831c66d + words[25]; y = f2(h,a,b); c += x; g = x + y;
    x = f + f1(c,d,e) + 0xb00327c8 + words[26]; y = f2(g,h,a); b += x; f = x + y;
    x = e + f1(b,c,d) + 0xbf597fc7 + words[27]; y = f2(f,g,h); a += x; e = x + y;
    x = d + f1(a,b,c) + 0xc6e00bf3 + words[28]; y = f2(e,f,g); h += x; d = x + y;
    x = c + f1(h,a,b) + 0xd5a79147 + words[29]; y = f2(d,e,f); g += x; c = x + y;
    x = b + f1(g,h,a) + 0x06ca6351 + words[30]; y = f2(c,d,e); f += x; b = x + y;
    x = a + f1(f,g,h) + 0x14292967 + words[31]; y = f2(b,c,d); e += x; a = x + y;

    // extend to 40 words
    for (; i < 40; i++) {
        words[i] = words[i-16] +
                (ROTATERIGHT(words[i-15],  7) ^ ROTATERIGHT(words[i-15], 18) ^ (words[i-15] >>  3)) +
                words[i-7] +
                (ROTATERIGHT(words[i- 2], 17) ^ ROTATERIGHT(words[i- 2], 19) ^ (words[i- 2] >> 10));
    }

    // fifth round
    x = h + f1(e,f,g) + 0x27b70a85 + words[32]; y = f2(a,b,c); d += x; h = x + y;
    x = g + f1(d,e,f) + 0x2e1b2138 + words[33]; y = f2(h,a,b); c += x; g = x + y;
    x = f + f1(c,d,e) + 0x4d2c6dfc + words[34]; y = f2(g,h,a); b += x; f = x + y;
    x = e + f1(b,c,d) + 0x53380d13 + words[35]; y = f2(f,g,h); a += x; e = x + y;
    x = d + f1(a,b,c) + 0x650a7354 + words[36]; y = f2(e,f,g); h += x; d = x + y;
    x = c + f1(h,a,b) + 0x766a0abb + words[37]; y = f2(d,e,f); g += x; c = x + y;
    x = b + f1(g,h,a) + 0x81c2c92e + words[38]; y = f2(c,d,e); f += x; b = x + y;
    x = a + f1(f,g,h) + 0x92722c85 + words[39]; y = f2(b,c,d); e += x; a = x + y;

    // extend to 48 words
    for (; i < 48; i++) {
        words[i] = words[i-16] +
                (ROTATERIGHT(words[i-15],  7) ^ ROTATERIGHT(words[i-15], 18) ^ (words[i-15] >>  3)) +
                words[i-7] +
                (ROTATERIGHT(words[i- 2], 17) ^ ROTATERIGHT(words[i- 2], 19) ^ (words[i- 2] >> 10));
    }

    // sixth round
    x = h + f1(e,f,g) + 0xa2bfe8a1 + words[40]; y = f2(a,b,c); d += x; h = x + y;
    x = g + f1(d,e,f) + 0xa81a664b + words[41]; y = f2(h,a,b); c += x; g = x + y;
    x = f + f1(c,d,e) + 0xc24b8b70 + words[42]; y = f2(g,h,a); b += x; f = x + y;
    x = e + f1(b,c,d) + 0xc76c51a3 + words[43]; y = f2(f,g,h); a += x; e = x + y;
    x = d + f1(a,b,c) + 0xd192e819 + words[44]; y = f2(e,f,g); h += x; d = x + y;
    x = c + f1(h,a,b) + 0xd6990624 + words[45]; y = f2(d,e,f); g += x; c = x + y;
    x = b + f1(g,h,a) + 0xf40e3585 + words[46]; y = f2(c,d,e); f += x; b = x + y;
    x = a + f1(f,g,h) + 0x106aa070 + words[47]; y = f2(b,c,d); e += x; a = x + y;

    // extend to 56 words
    for (; i < 56; i++) {
        words[i] = words[i-16] +
                (ROTATERIGHT(words[i-15],  7) ^ ROTATERIGHT(words[i-15], 18) ^ (words[i-15] >>  3)) +
                words[i-7] +
                (ROTATERIGHT(words[i- 2], 17) ^ ROTATERIGHT(words[i- 2], 19) ^ (words[i- 2] >> 10));
    }

    // seventh round
    x = h + f1(e,f,g) + 0x19a4c116 + words[48]; y = f2(a,b,c); d += x; h = x + y;
    x = g + f1(d,e,f) + 0x1e376c08 + words[49]; y = f2(h,a,b); c += x; g = x + y;
    x = f + f1(c,d,e) + 0x2748774c + words[50]; y = f2(g,h,a); b += x; f = x + y;
    x = e + f1(b,c,d) + 0x34b0bcb5 + words[51]; y = f2(f,g,h); a += x; e = x + y;
    x = d + f1(a,b,c) + 0x391c0cb3 + words[52]; y = f2(e,f,g); h += x; d = x + y;
    x = c + f1(h,a,b) + 0x4ed8aa4a + words[53]; y = f2(d,e,f); g += x; c = x + y;
    x = b + f1(g,h,a) + 0x5b9cca4f + words[54]; y = f2(c,d,e); f += x; b = x + y;
    x = a + f1(f,g,h) + 0x682e6ff3 + words[55]; y = f2(b,c,d); e += x; a = x + y;

    // extend to 64 words
    for (; i < 64; i++) {
        words[i] = words[i-16] +
                (ROTATERIGHT(words[i-15],  7) ^ ROTATERIGHT(words[i-15], 18) ^ (words[i-15] >>  3)) +
                words[i-7] +
                (ROTATERIGHT(words[i- 2], 17) ^ ROTATERIGHT(words[i- 2], 19) ^ (words[i- 2] >> 10));
    }

    // eigth round
    x = h + f1(e,f,g) + 0x748f82ee + words[56]; y = f2(a,b,c); d += x; h = x + y;
    x = g + f1(d,e,f) + 0x78a5636f + words[57]; y = f2(h,a,b); c += x; g = x + y;
    x = f + f1(c,d,e) + 0x84c87814 + words[58]; y = f2(g,h,a); b += x; f = x + y;
    x = e + f1(b,c,d) + 0x8cc70208 + words[59]; y = f2(f,g,h); a += x; e = x + y;
    x = d + f1(a,b,c) + 0x90befffa + words[60]; y = f2(e,f,g); h += x; d = x + y;
    x = c + f1(h,a,b) + 0xa4506ceb + words[61]; y = f2(d,e,f); g += x; c = x + y;
    x = b + f1(g,h,a) + 0xbef9a3f7 + words[62]; y = f2(c,d,e); f += x; b = x + y;
    x = a + f1(f,g,h) + 0xc67178f2 + words[63]; y = f2(b,c,d); e += x; a = x + y;

    // update hash
    m_hash[0] += a;
    m_hash[1] += b;
    m_hash[2] += c;
    m_hash[3] += d;
    m_hash[4] += e;
    m_hash[5] += f;
    m_hash[6] += g;
    m_hash[7] += h;
}

void Sha256::processBuffer()
{
    /// the input bytes are considered as bits strings, where the first bit is the most significant bit of the byte
    /// - append "1" bit to message
    /// - append "0" bits until message length in bit mod 512 is 448
    /// - append length as 64 bit integer

    // number of bits
    quint64 paddedLength = m_bufferSize * 8;

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

    quint64 i;
    for (i = m_bufferSize + 1; i < BLOCK_SIZE; i++) {
        m_buffer[i] = 0;
    }
    for (; i < paddedLength; i++) {
        extra[i - BLOCK_SIZE] = 0;
    }

    // add message length in bits as 64 bit number
    quint64 msgBits = 8 * (m_numBytes + m_bufferSize);

    // find right position
    std::array<quint8, BLOCK_SIZE>::iterator addLength;
    if (paddedLength < BLOCK_SIZE) {
        addLength = m_buffer.begin() + paddedLength;
    }
    else {
        addLength = extra.begin() + paddedLength - BLOCK_SIZE;
    }

    // must be big endian
    *addLength++ = static_cast<quint8>((msgBits >> 56) & 0xFF);
    *addLength++ = static_cast<quint8>((msgBits >> 48) & 0xFF);
    *addLength++ = static_cast<quint8>((msgBits >> 40) & 0xFF);
    *addLength++ = static_cast<quint8>((msgBits >> 32) & 0xFF);
    *addLength++ = static_cast<quint8>((msgBits >> 24) & 0xFF);
    *addLength++ = static_cast<quint8>((msgBits >> 16) & 0xFF);
    *addLength++ = static_cast<quint8>((msgBits >>  8) & 0xFF);
    *addLength   = static_cast<quint8>( msgBits        & 0xFF);

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
