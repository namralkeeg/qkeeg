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
#ifndef HASHALGORITHM_HPP
#define HASHALGORITHM_HPP

#include <QtGlobal>
#include <QByteArray>
#include <QIODevice>
#include <QString>
#include <type_traits>

#ifndef HASH_BLOCK_BUFFER_SIZE
    // Block of bytes to process per file read.
    // each cycle processes about 1 MByte (divisible by 144 => improves Keccak/SHA3 performance)
    #define HASH_BLOCK_BUFFER_SIZE Q_INT64_C(1032192) // 144 * 7 * 1024
#endif

namespace qkeeg { namespace hashing {

class HashAlgorithm
{
    Q_GADGET

public:
    //! Virtual Destructor
    virtual ~HashAlgorithm();

    //! Compute hash of a byte array
    QByteArray computeHash(const QByteArray &data);
    QByteArray computeHash(const QByteArray &data, const qint32 &offset, const qint32 &count);

    //! Comput Hash of a stream
    QByteArray computeHash(QIODevice &instream);

    //! Make sure everything is setup, or reset.
    virtual void initialize() = 0;

    //! Size of the return hash in bits.
    virtual quint32 hashSize() = 0;
    //! Get the hash value as byte array.
    QByteArray hashValue() const;
    //! Get the hash value as a hex string.
    QString hashValueToHex(bool useUpperCase = true, bool insertSpaces = false);

    //! Compute Hash of a string, excluding final zero. Returns the hash as a hex string.
    QString operator ()(const QString &text);
    //! Compute hash of a byte array
    QByteArray operator ()(const QByteArray &data);

protected:
    //! Protected constructor for abstract class.
    HashAlgorithm();

    //! Hashing function that does the work. Must be implemented in the derived class.
    virtual void hashCore(const void* data, const qint64 &offset, const qint64 &count) = 0;

    //! This is called to finalize the hash computation.
    virtual QByteArray hashFinal() = 0;

    QString byteArrayToHex(const QByteArray &data, bool useUpperCase = true, bool insertSpaces = false);

protected:
    const qint64 m_blockSizeBuffer = HASH_BLOCK_BUFFER_SIZE;
    QByteArray m_hashValue;

private:
    static_assert(std::is_same<quint8, unsigned char>::value,
                  "quint8 is required to be implemented as unsigned char!");
};

} // namespace hashing
} // namespace qkeeg

#endif // HASHALGORITHM_HPP
