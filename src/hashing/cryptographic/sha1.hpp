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
#ifndef SHA1_HPP
#define SHA1_HPP

#include "../hashalgorithm.hpp"
#include <array>

namespace qkeeg { namespace hashing { namespace cryptographic {

class Sha1 : public HashAlgorithm
{
    Q_GADGET

public:
    Sha1();

    // HashAlgorithm interface
public:
    virtual void initialize() override;
    virtual quint32 hashSize() override;

protected:
    virtual void hashCore(const void *data, const qint64 &offset, const qint64 &count) override;
    virtual QByteArray hashFinal() override;

private:
    static const quint32 m_hashSize = std::numeric_limits<quint8>::digits * 20;

    /// split into 64 byte blocks (=> 512 bits)
    static const quint32 BLOCK_SIZE = 512 / 8;
    static const quint32 NUM_HASH_VALUES = 20 / 4;

    /// size of processed data in bytes
    quint64 m_numBytes;
    /// valid bytes in m_buffer
    quint64 m_bufferSize;
    /// bytes not processed yet
    std::array<quint8, BLOCK_SIZE> m_buffer;
    /// hash, stored as integers
    std::array<quint32, NUM_HASH_VALUES> m_hash;

    /// process 64 bytes
    void processBlock(const void *data);

    /// process everything left in the internal buffer
    void processBuffer();
};

} // namespace cryptographic
} // namespace hashing
} // namespace qkeeg

#endif // SHA1_HPP
