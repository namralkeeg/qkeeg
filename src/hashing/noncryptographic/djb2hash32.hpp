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
#ifndef DJB2HASH32_HPP
#define DJB2HASH32_HPP

#include "../hashalgorithm.hpp"

namespace qkeeg { namespace hashing { namespace noncryptographic {

class Djb2Hash32 : public HashAlgorithm
{
    Q_GADGET

public:
    Djb2Hash32();

    // HashAlgorithm interface
public:
    virtual void initialize() override;
    virtual quint32 hashSize() override;

protected:
    virtual void hashCore(const void *data, const qint64 &offset, const qint64 &count) override;
    virtual QByteArray hashFinal() override;

private:
    static const quint32 m_hashSize = std::numeric_limits<quint32>::digits;
    static const quint32 m_defaultSeed = UINT32_C(5381);
    quint32 m_hash;
};

} // namespace noncryptographic
} // namespace hashing
} // namespace qkeeg

#endif // DJB2HASH32_HPP
