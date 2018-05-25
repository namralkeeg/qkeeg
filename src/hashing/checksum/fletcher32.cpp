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
#include "fletcher32.hpp"
#include "../../common/endian.hpp"

namespace qkeeg { namespace hashing { namespace checksum {

Fletcher32::Fletcher32() : m_sum1(m_seed), m_sum2(m_seed)
{
    initialize();
}

void Fletcher32::initialize()
{
    m_sum1 = m_seed;
    m_sum2 = m_seed;
    m_hashValue.clear();
}

quint32 Fletcher32::hashSize()
{
    return m_hashSize;
}

void Fletcher32::hashCore(const void *data, const qint64 &offset, const qint64 &count)
{
    const quint8  *temp = reinterpret_cast<const quint8*>(data) + offset;
    const quint16 *current = reinterpret_cast<const quint16*>(temp);

    quint64 words = count / 2;
    quint16 tlen;
    while (words) {
        tlen = (words >= 359) ? 359 : words;
        words -= tlen;
        do {
            m_sum1 += common::from_unaligned<quint16>(current++);
            m_sum2 += m_sum1;
            tlen--;
        }
        while (tlen);

        m_sum1 = (m_sum1 & UINT32_C(0xFFFF)) + (m_sum1 >> 16);
        m_sum2 = (m_sum2 & UINT32_C(0xFFFF)) + (m_sum2 >> 16);
    }
}

QByteArray Fletcher32::hashFinal()
{
    m_sum1 = (m_sum1 & UINT32_C(0xFFFF)) + (m_sum1 >> 16);
    m_sum2 = (m_sum2 & UINT32_C(0xFFFF)) + (m_sum2 >> 16);

    quint32 hash = (m_sum2 << 16) | m_sum1;

    QByteArray buffer(sizeof(hash), char(0));
    common::to_unaligned<quint32>(hash, buffer.data());
    return buffer;
}

} // namespace checksum
} // namespace hashing
} // namespace qkeeg
