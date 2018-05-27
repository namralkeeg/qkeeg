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
#ifndef XXHASH32_HPP
#define XXHASH32_HPP

#include "../hashalgorithm.hpp"
#include <array>

namespace qkeeg { namespace hashing { namespace noncryptographic {

class XxHash32 : public HashAlgorithm
{
    Q_GADGET

public:
    XxHash32(quint32 seed = 0);
    virtual ~XxHash32();

    // HashAlgorithm interface
public:
    virtual void initialize() override;
    virtual quint32 hashSize() override;

protected:
    virtual void hashCore(const void *data, const qint64 &offset, const qint64 &count) override;
    virtual QByteArray hashFinal() override;

private:
    static const quint32 m_hashSize = std::numeric_limits<quint32>::digits;

    /// magic constants
    static const quint32 Prime1 = UINT32_C(2654435761);
    static const quint32 Prime2 = UINT32_C(2246822519);
    static const quint32 Prime3 = UINT32_C(3266489917);
    static const quint32 Prime4 =  UINT32_C(668265263);
    static const quint32 Prime5 =  UINT32_C(374761393);

    /// temporarily store up to 15 bytes between multiple add() calls
    static const quint32 MaxBufferSize = 15+1;

    /// internal state and temporary buffer
    std::array<quint32, 4> m_state; // state[2] == seed if totalLength < MaxBufferSize
    std::array<quint8, MaxBufferSize> m_buffer;
    quint32  m_bufferSize;
    qint64   m_totalLength;
    quint32  m_seed;

    /// process a block of 4x4 bytes, this is the main part of the XXHash32 algorithm
    void process(const void* data, quint32 &state0, quint32 &state1, quint32 &state2, quint32 &state3);
};

} // namespace noncryptographic
} // namespace hashing
} // namespace qkeeg

#endif // XXHASH32_HPP
