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
#include "hashalgorithm.hpp"
#include <memory>

namespace qkeeg { namespace hashing {

HashAlgorithm::~HashAlgorithm()
{
    if ((m_hashValue != nullptr) && !m_hashValue.isNull()) {
        m_hashValue.fill(char(0));
        m_hashValue.clear();
    }
}

QByteArray HashAlgorithm::computeHash(const QByteArray &data)
{
    return computeHash(data, 0, data.size());
}

QByteArray HashAlgorithm::computeHash(const QByteArray &data, const qint32 &offset, const qint32 &count)
{
    if ((data == nullptr) || (data.size() < 1)) {
        throw QString("Data buffer is null or empty.");
    }
    if (offset < 0) {
        throw QString("Invalid offset.");
    }
    if ((count < 0) || (count > data.size())) {
        throw QString("Invalid count.");
    }
    if ((data.size() - count) < offset) {
        throw QString("Invalid offset and count specified.");
    }

    initialize();
    hashCore(data.data(), offset, count);
    m_hashValue = hashFinal();
    return m_hashValue;
}

QByteArray HashAlgorithm::computeHash(QIODevice &instream)
{
    if (!instream.isReadable()) {
        return QByteArray();
    }
    else
    {
        instream.seek(0);
        qint64 bytesToRead = instream.size();
        qint64 blockSize = (m_blockSizeBuffer > bytesToRead) ? bytesToRead : m_blockSizeBuffer;

        // smart buffer array.
        std::unique_ptr<char[]> buffer = std::make_unique<char[]>(blockSize);
        initialize();

        qint64 numBytesRead = 0;
        while (bytesToRead > 0)
        {
            if (bytesToRead > blockSize) {
                numBytesRead = instream.read(buffer.get(), blockSize);
            }
            else {
                numBytesRead = instream.read(buffer.get(), bytesToRead);
            }

            // There was an error reading the IO device.
            if (numBytesRead <= 0) {
                return QByteArray();
            }

            hashCore(reinterpret_cast<const void*>(buffer.get()), 0, numBytesRead);
            bytesToRead -= numBytesRead;
        }

        m_hashValue = hashFinal();
        return m_hashValue;
    }
}

QByteArray HashAlgorithm::hashValue() const
{
    return m_hashValue;
}

QString HashAlgorithm::hashValueToHex(bool useUpperCase, bool insertSpaces)
{
    return byteArrayToHex(m_hashValue, useUpperCase, insertSpaces);
}

QString HashAlgorithm::operator ()(const QString &text)
{
    QByteArray temp = text.toUtf8();
    QByteArray hash = computeHash(temp, 0, temp.size());
    return byteArrayToHex(hash);
}

QByteArray HashAlgorithm::operator ()(const QByteArray &data)
{
    return computeHash(data, 0, data.size());
}

HashAlgorithm::HashAlgorithm()
{

}

QString HashAlgorithm::byteArrayToHex(const QByteArray &data, bool useUpperCase, bool insertSpaces)
{
    QByteArray temp;
    temp = (insertSpaces) ? data.toHex(' ') : data.toHex();
    if(useUpperCase) {
        temp = temp.toUpper();
    }

    return QString(temp);
}

} // namespace hashing
} // namespace qkeeg
