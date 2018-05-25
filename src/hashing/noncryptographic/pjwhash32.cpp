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
#include "pjwhash32.hpp"
#include "../../common/endian.hpp"

namespace qkeeg { namespace hashing { namespace noncryptographic {

PJWHash32::PJWHash32() : HashAlgorithm()
{
    initialize();
}

void PJWHash32::initialize()
{
    m_hash = 0;
    m_hashValue.clear();
}

quint32 PJWHash32::hashSize()
{
    return BitsInUnsignedInt;
}

void PJWHash32::hashCore(const void *data, const qint64 &offset, const qint64 &count)
{
    const quint8 *current = reinterpret_cast<const quint8*>(data) + offset;
    quint32 test = 0;

    for(qint64 i = 0; i < count; ++current, ++i)
    {
        m_hash = (m_hash << OneEighth) + *current;

        if((test = m_hash & HighBits) != 0) {
            m_hash = ((m_hash ^ (test >> ThreeQuarters)) & (~HighBits));
        }
    }
}

QByteArray PJWHash32::hashFinal()
{
    QByteArray buffer(sizeof(m_hash), char(0));
    common::to_unaligned<quint32>(m_hash, buffer.data());
    return buffer;
}

} // namespace noncryptographic
} // namespace hashing
} // namespace qkeeg
