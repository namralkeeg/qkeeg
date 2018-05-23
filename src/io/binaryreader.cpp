/*
 * Copyright (C) 2017 Larry Lopez
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
#include <io/binaryreader.hpp>
#include <common/endian.hpp>
#include <array>

namespace qkeeg { namespace io {

BinaryReader::BinaryReader(QIODevice &readDevice, const QSysInfo::Endian &byteOrder) :
    m_baseDevice(&readDevice), m_codec(QTextCodec::codecForName(m_defaultEncoding)),
    m_byteOrder(byteOrder)
{
    m_doswap = (QSysInfo::ByteOrder != m_byteOrder) ? true : false;
}

BinaryReader::BinaryReader(QIODevice &readDevice, QTextCodec *codec, const QSysInfo::Endian &byteOrder) :
    m_baseDevice(&readDevice), m_codec(codec), m_byteOrder(byteOrder)
{
    m_doswap = (QSysInfo::ByteOrder != m_byteOrder) ? true : false;
    if (m_codec == nullptr)
        m_codec = QTextCodec::codecForName(m_defaultEncoding);
}

QTextCodec *BinaryReader::codec() const
{
    return m_codec;
}

void BinaryReader::setCodec(QTextCodec *codec)
{
    if (codec != nullptr)
        m_codec = codec;
}

QIODevice *BinaryReader::baseDevice()
{
    return m_baseDevice;
}

void BinaryReader::setBaseDevice(QIODevice *baseDevice)
{
    m_baseDevice = baseDevice;
}

QSysInfo::Endian BinaryReader::byteOrder() const
{
    return m_byteOrder;
}

void BinaryReader::setByteOrder(const QSysInfo::Endian &byteOrder)
{
    m_byteOrder = byteOrder;
    m_doswap = (QSysInfo::ByteOrder != m_byteOrder) ? true : false;
}

BinaryReader::Status BinaryReader::status() const
{
    return m_status;
}

qint32 BinaryReader::read(QByteArray &buffer, const qint32 &index, const qint32 &count)
{
    try
    {
        if (index < 0 || count < 0)
        {
            return -1;
        }
        else if (count == 0)
        {
            buffer.resize(0);
            return 0;
        }

        if (m_baseDevice->isReadable())
        {
            if (count + index > buffer.size())
                buffer.resize(count + index);

            qint32 bytesRead = readBlock(buffer.data(), index, count);

            if ((bytesRead != -1) && (bytesRead < count))
                buffer.resize(bytesRead);

            return bytesRead;
        }
    }
    catch(...)
    {
        return -1;
    }

    return -1;
}

qint32 BinaryReader::read(QVector<quint8> &buffer, const qint32 &index, const qint32 &count)
{
    try
    {
        if (index < 0 || count < 0)
        {
            return -1;
        }
        else if (count == 0)
        {
            buffer.resize(0);
            return 0;
        }

        if (m_baseDevice->isReadable())
        {
            if (count + index > buffer.size())
                buffer.resize(count + index);

            qint32 bytesRead = readBlock(buffer.data(), index, count);

            if ((bytesRead != -1) && (bytesRead < count))
                buffer.resize(bytesRead);

            return bytesRead;
        }
    }
    catch(...)
    {
        return -1;
    }

    return -1;
}

qint32 BinaryReader::read7BitEncodedInt()
{
    qint32 count = 0;
    qint32 shift = 0;
    quint8 b;

    do
    {
        // Check for a corrupted stream.  Read a max of 5 bytes.
        if (shift == (5 * 7)) {
            throw QString("Invalid format for 7 bit encoded int.");
        }

        b = readByte();
        count |= (b & 0x7F) << shift;
        shift += 7;
    }
    while ((b & 0x80) != 0);

    // Compressed int's are read in Little-Endian then converted to native.
    return common::little_to_native<qint32>(count);
}

bool BinaryReader::readBoolean()
{
    return (readByte() != 0);
}

quint8 BinaryReader::readByte()
{
    quint8 buffer = 0;

    if (readBlock(&buffer, 0, sizeof(buffer)) != sizeof(buffer)) {
        buffer = 0;
    }

    return buffer;
}

QByteArray BinaryReader::readBytes(const qint32 &count)
{
    QByteArray qba(count, char(0));
    qint32 numRead = 0;
    qint32 byteCount = count;
    qint32 n;

    do
    {
        n = readBlock(qba.data(), numRead, byteCount);
        if (n <= 0)
            break;
        numRead += n;
        byteCount -= n;
    }
    while (byteCount > 0);

    if (numRead != count)
        qba.resize(numRead);

    return qba;
}

double BinaryReader::readDouble()
{
    std::array<quint8, sizeof(quint64)> buffer;
    double value = 0.0;
    quint64 temp;

    if (readBlock(buffer.data(), 0, sizeof(quint64)) != sizeof(quint64))
    {
        value = 0.0;
    }

    if (m_byteOrder == QSysInfo::Endian::LittleEndian)
    {
        temp = static_cast<quint64>(
                 static_cast<quint64>(buffer[0]) |
                (static_cast<quint64>(buffer[1]) << 8) |
                (static_cast<quint64>(buffer[2]) << 16) |
                (static_cast<quint64>(buffer[3]) << 24) |
                (static_cast<quint64>(buffer[4]) << 32) |
                (static_cast<quint64>(buffer[5]) << 40) |
                (static_cast<quint64>(buffer[6]) << 48) |
                (static_cast<quint64>(buffer[7]) << 56)
                );
    }
    else
    {
        temp = static_cast<quint64>(
                 static_cast<quint64>(buffer[7]) |
                (static_cast<quint64>(buffer[6]) << 8) |
                (static_cast<quint64>(buffer[5]) << 16) |
                (static_cast<quint64>(buffer[4]) << 24) |
                (static_cast<quint64>(buffer[3]) << 32) |
                (static_cast<quint64>(buffer[2]) << 40) |
                (static_cast<quint64>(buffer[1]) << 48) |
                (static_cast<quint64>(buffer[0]) << 56)
                );
    }

    std::memcpy(&value, &temp, sizeof(temp));

    return value;

//    double buffer = 0.0;

//    if (readBlock(&buffer, 0, sizeof(buffer)) != sizeof(buffer))
//    {
//        buffer = 0.0;
//    }
//    else
//    {
//        if (m_doswap)
//        {
//            union
//            {
//                double val1;
//                quint64 val2;
//            } x;

//            x.val2 = common::swap<quint64>(*reinterpret_cast<quint64 *>(&buffer));
//            buffer = x.val1;
//        }
//    }

//    return buffer;
}

float BinaryReader::readFloat()
{
    std::array<quint8, sizeof(quint32)> buffer;
    float value = 0.0;
    quint32 temp;

    if (readBlock(buffer.data(), 0, sizeof(quint32)) != sizeof(quint32))
    {
        value = 0.0;
    }

    if (m_byteOrder == QSysInfo::Endian::LittleEndian)
    {
        temp = static_cast<quint32>(buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24));
    }
    else
    {
        temp = static_cast<quint32>(buffer[3] | (buffer[2] << 8) | (buffer[1] << 16) | (buffer[0] << 24));
    }

    std::memcpy(&value, &temp, sizeof(temp));

    return value;
//    float buffer = 0.0;

//    if (readBlock(&buffer, 0, sizeof(buffer)) != sizeof(buffer))
//    {
//        buffer = 0.0;
//    }
//    else
//    {
//        if (m_doswap)
//        {
//            union
//            {
//                float val1;
//                quint32 val2;
//            } x;

//            x.val2 = common::swap<quint32>(*reinterpret_cast<quint32 *>(&buffer));
//            buffer = x.val1;
//        }
//    }

//    return buffer;
}

qint16 BinaryReader::readInt16()
{
    qint16 buffer;

    if (readBlock(&buffer, 0, sizeof(buffer)) != sizeof(buffer))
    {
        buffer = 0;
    }
    else
    {
        if (m_doswap) {
            buffer = common::swap<qint16>(buffer);
        }
    }

    return buffer;
}

qint32 BinaryReader::readInt32()
{
    qint32 buffer;

    if (readBlock(&buffer, 0, sizeof(buffer)) != sizeof(buffer))
    {
        buffer = 0;
    }
    else
    {
        if (m_doswap) {
            buffer = common::swap<qint32>(buffer);
        }
    }

    return buffer;
}

qint64 BinaryReader::readInt64()
{
    qint64 buffer;

    if (readBlock(&buffer, 0, sizeof(buffer)) != sizeof(buffer))
    {
        buffer = 0;
    }
    else
    {
        if (m_doswap) {
            buffer = common::swap<qint64>(buffer);
        }
    }

    return buffer;
}

qint8 BinaryReader::readSByte()
{
    qint8 buffer;

    if (readBlock(&buffer, 0, sizeof(buffer)) != sizeof(buffer))
    {
        buffer = 0;
    }

    return buffer;
}

float BinaryReader::readSingle()
{
    return readFloat();
}

QString BinaryReader::readString()
{
    qint32 numRead = 0;
    qint32 byteCount;
    qint32 n;

    byteCount = read7BitEncodedInt();

    if (byteCount < 0)
        throw QString("Invalid string length.");

    if (byteCount == 0)
        return QString();

    QByteArray qba(byteCount, char(0));
    do
    {
        n = readBlock(qba.data(), numRead, byteCount);
        if (n <= 0)
            break;
        numRead += n;
        byteCount -= n;
    }
    while (byteCount > 0);

    if (numRead != byteCount)
        qba.resize(numRead);

    // Return the string converted to Unicode from the set codec.
    return m_codec->toUnicode(qba);
}

quint16 BinaryReader::readUInt16()
{
    quint16 buffer;

    if (readBlock(&buffer, 0, sizeof(buffer)) != sizeof(buffer))
    {
        buffer = 0;
    }
    else
    {
        if (m_doswap) {
            buffer = common::swap<quint16>(buffer);
        }
    }

    return buffer;
}

quint32 BinaryReader::readUInt32()
{
    quint32 buffer;

    if (readBlock(&buffer, 0, sizeof(buffer)) != sizeof(buffer))
    {
        buffer = 0;
    }
    else
    {
        if (m_doswap) {
            buffer = common::swap<quint32>(buffer);
        }
    }

    return buffer;
}

quint64 BinaryReader::readUInt64()
{
    quint64 buffer;

    if (readBlock(&buffer, 0, sizeof(buffer)) != sizeof(buffer))
    {
        buffer = 0;
    }
    else
    {
        if (m_doswap) {
            buffer = common::swap<quint64>(buffer);
        }
    }

    return buffer;
}

QString BinaryReader::readBString()
{
    quint8 size = readByte();

    if (size <= 0)
        return QString();

    QByteArray qba = readBytes(size);

    // Return the string converted to unicode.
    return QString(m_codec->toUnicode(qba));
}

QString BinaryReader::readBZString()
{
    quint8 size = readByte();

    if (size <= 0)
        return QString();

    QByteArray qba = readBytes(size);
    // Remove the terminating null char.
    qba.resize(size-1);

    // Return the string converted to unicode.
    return QString(m_codec->toUnicode(qba));
}

QString BinaryReader::readWString()
{
    quint16 size = readUInt16();

    if (size <= 0)
        return QString();

    QByteArray qba = readBytes(size);

    // Return the string converted to unicode.
    return QString(m_codec->toUnicode(qba));
}

QString BinaryReader::readWZString()
{
    quint16 size = readUInt16();

    if (size <= 0)
        return QString();

    QByteArray qba = readBytes(size);
    // Remove the terminating null char.
    qba.resize(size-1);

    // Return the string converted to unicode.
    return QString(m_codec->toUnicode(qba));
}

QString BinaryReader::readZString()
{
    QByteArray qba;
    char c;
    qint64 bytesRead = 0;

    while (readBlock(&c, 0, sizeof(char)) > 0)
    {
        if (c != char(0))
            qba += c;
        else
            break;

        bytesRead++;
    }

    // Return the string converted to unicode.
    return QString(m_codec->toUnicode(qba));
}

qint64 BinaryReader::readBlock(void *buffer, const qint64 &index, const qint64 &count)
{
    QMutexLocker lock(&m_readMutex);
    qint64 bytesRead = -1;
    try
    {
        if (index < 0 || count < 0) {
            return -1;
        }
        else if (count == 0) {
            return 0;
        }

        char *current = reinterpret_cast<char *>(buffer) + index;

        if (m_baseDevice->isReadable())
        {
            bytesRead = m_baseDevice->read(current, count);

            if ((bytesRead != -1) && (bytesRead < count)) {
                m_status = ReadPastEnd;
            }

            return bytesRead;
        }
    }
    catch(...)
    {
        return bytesRead;
    }

    return bytesRead;
}

} // namespace io
} // namespace qkeeg
