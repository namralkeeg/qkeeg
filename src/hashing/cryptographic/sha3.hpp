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
#ifndef SHA3_HPP
#define SHA3_HPP

#include "../hashalgorithm.hpp"
#include <array>

namespace qkeeg { namespace hashing { namespace cryptographic {

class Sha3 : public HashAlgorithm
{
    Q_GADGET

public:
    enum class Bits : quint32 { Bits224 = 224, Bits256 = 256, Bits384 = 384, Bits512 = 512 };

    explicit Sha3(Bits bits = Bits::Bits256);
    virtual ~Sha3();

    // HashAlgorithm interface
public:
    virtual void initialize() override;
    virtual quint32 hashSize() override;

protected:
    virtual void hashCore(const void *data, const qint64 &offset, const qint64 &count) override;
    virtual QByteArray hashFinal() override;

private:
    /// 1600 bits, stored as 25x64 bit, BlockSize is no more than 1152 bits (Keccak224)
    static const quint32 StateSize = 1600 / (8 * 8);
    static const quint32 MaxBlockSize = 200 - 2 * (224 / 8);

    /// hash
    std::array<quint64, StateSize> m_hash;
    /// size of processed data in bytes
    quint64 m_numBytes;
    /// block size (less or equal to MaxBlockSize)
    quint64 m_blockSize;
    /// valid bytes in m_buffer
    quint64 m_bufferSize;
    /// bytes not processed yet
    std::array<quint8, MaxBlockSize> m_buffer{};
    /// variant
    Bits m_bits;

    /// process a full block
    void processBlock(const void* data);
    /// process everything left in the internal buffer
    void processBuffer();
};

} // namespace cryptographic
} // namespace hashing
} // namespace qkeeg

#endif // SHA3_HPP
