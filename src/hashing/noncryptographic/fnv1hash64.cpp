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
#include "fnv1hash64.hpp"
#include "../../common/endian.hpp"

namespace qkeeg { namespace hashing { namespace noncryptographic {

Fnv1Hash64::Fnv1Hash64()
{
    initialize();
}

void Fnv1Hash64::initialize()
{
    m_hash = m_offsetBasis;
    m_hashValue.clear();
}

quint32 Fnv1Hash64::hashSize()
{
    return m_hashSize;
}

void Fnv1Hash64::hashCore(const void *data, const qint64 &offset, const qint64 &count)
{
    const quint8 *current = reinterpret_cast<const quint8*>(data) + offset;

    for (qint64 i = 0; i < count; ++current, ++i)
    {
        m_hash = (m_fnvPrime * m_hash) ^ *current;
    }
}

QByteArray Fnv1Hash64::hashFinal()
{
    QByteArray buffer(sizeof(m_hash), char(0));
    qkeeg::common::to_unaligned<quint64>(m_hash, buffer.data());
    return buffer;
}

} // namespace noncryptographic
} // namespace hashing
} // namespace qkeeg
