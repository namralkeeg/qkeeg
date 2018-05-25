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
#include "adler32.hpp"
#include "../../common/endian.hpp"

namespace qkeeg { namespace hashing { namespace checksum {

#define DO1(buf,i)  {a += (buf)[i]; b += a;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);

Adler32::Adler32()
{

}

void Adler32::initialize()
{
    m_hash = m_seed;
    m_hashValue.clear();
}

quint32 Adler32::hashSize()
{
    return m_hashSize;
}

void Adler32::hashCore(const void *data, const qint64 &offset, const qint64 &count)
{
    quint32 a = m_hash & UINT32_C(0xFFFF);
    quint32 b = (m_hash >> 16) & UINT32_C(0xFFFF);
    const quint8 *current = reinterpret_cast<const quint8*>(data) + offset;

    if (data == nullptr) {
        m_hash = 1L;
    }
    else if (count == 1) {
        a += *current;
        if (a >= m_modAdler) {
            a -= m_modAdler;
        }
        b += a;
        if (b >= m_modAdler) {
            b -= m_modAdler;
        }

        m_hash = a | (b << 16);
    }
    else {
        quint64 length = count;
        quint64 k;
        while (length > 0) {

            k = length < m_nmax ? length : m_nmax;

            length -= k;
            while (k >= 16) {
                DO16(current);
                current += 16;
                k -= 16;
            }

            if (k != 0) {
                do {
                    a += *current++;
                    b += a;
                }
                while (--k);
            }

            a %= m_modAdler;
            b %= m_modAdler;
        }

        m_hash = a | (b << 16);
    }
}

QByteArray Adler32::hashFinal()
{
    QByteArray buffer(sizeof(m_hash), char(0));
    common::to_unaligned<quint32>(m_hash, buffer.data());
    return buffer;
}

} // namespace checksum
} // namespace hashing
} // namespace qkeeg
