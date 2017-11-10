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

namespace qkeeg { namespace io {

BinaryReader::BinaryReader(QIODevice &readDevice, const QSysInfo::Endian &endian) :
    m_baseDevice(&readDevice), m_codec(QTextCodec::codecForName(m_defaultEncoding)),
    m_endian(endian), m_dataStream(m_baseDevice)
{
    m_dataStream.setByteOrder(static_cast<QDataStream::ByteOrder>(endian));
    m_dataStream.setVersion(QDATASTREAM_DEFAULT_VERSION);
}

BinaryReader::BinaryReader(QIODevice &readDevice, QTextCodec *codec, const QSysInfo::Endian &endian) :
    m_baseDevice(&readDevice), m_codec(codec), m_endian(endian), m_dataStream(m_baseDevice)
{
    m_dataStream.setByteOrder(static_cast<QDataStream::ByteOrder>(endian));
    m_dataStream.setVersion(QDATASTREAM_DEFAULT_VERSION);
}

QTextCodec *BinaryReader::codec() const
{
    return m_codec;
}

void BinaryReader::setCodec(QTextCodec *codec)
{
    if (codec != nullptr)
    {
        m_codec = codec;
    }
}

QIODevice *BinaryReader::baseDevice()
{
    return m_baseDevice;
}

void BinaryReader::setBaseDevice(QIODevice *baseDevice)
{
    m_baseDevice = baseDevice;
    m_dataStream.setDevice(m_baseDevice);
}

QSysInfo::Endian BinaryReader::endian() const
{
    return m_endian;
}

void BinaryReader::setEndian(const QSysInfo::Endian &endian)
{
    m_endian = endian;
    m_dataStream.setByteOrder(static_cast<QDataStream::ByteOrder>(endian));
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

            qint32 bytesRead = m_dataStream.readRawData(buffer.data() + index, count);

            if ((bytesRead < count) && (bytesRead != -1))
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

            qint32 bytesRead = m_dataStream.readRawData(reinterpret_cast<char*>(buffer.data() + index), count);

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
    if (!m_baseDevice->isReadable())
            throw QString("Unable to read from device!");

    qint32 count = 0;
    qint32 shift = 0;
    quint8 b;

    do
    {
        // Check for a corrupted stream.  Read a max of 5 bytes.
        if (shift == (5 * 7))
            throw QString("Invalid format for 7 bit encoded int.");

        b = readByte();
        count |= (b & 0x7F) << shift;
        shift += 7;
    }
    while ((b & 0x80) != 0);

    return convertToEndian(count, m_endian);
}

bool BinaryReader::readBoolean()
{
    if (!m_baseDevice->isReadable())
            throw QString("Unable to read from device!");

    return (readByte() != 0);
}

quint8 BinaryReader::readByte()
{
    if (!m_baseDevice->isReadable())
            throw QString("Unable to read from device!");

    quint8 buffer;
    m_dataStream >> buffer;
    return buffer;
}

QByteArray BinaryReader::readBytes(const qint32 &count)
{
    if (!m_baseDevice->isReadable())
            throw QString("Unable to read from device!");

    QByteArray qba(count, char(0));
    qint32 numRead = 0;
    qint32 byteCount = count;
    qint32 n;

    do
    {
        n = m_dataStream.readRawData(qba.data()+numRead, byteCount);
        if (n == 0)
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
    if (!m_baseDevice->isReadable())
            throw QString("Unable to read from device!");

    double buffer;
    m_dataStream.setFloatingPointPrecision(QDataStream::DoublePrecision);
    m_dataStream >> buffer;
    return buffer;
}

float BinaryReader::readFloat()
{
    if (!m_baseDevice->isReadable())
            throw QString("Unable to read from device!");

    float buffer;
    m_dataStream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    m_dataStream >> buffer;
    return buffer;
}

qint16 BinaryReader::readInt16()
{
    if (!m_baseDevice->isReadable())
            throw QString("Unable to read from device!");

    qint16 buffer;
    m_dataStream >> buffer;
    return buffer;
}

qint32 BinaryReader::readInt32()
{
    if (!m_baseDevice->isReadable())
            throw QString("Unable to read from device!");

    qint32 buffer;
    m_dataStream >> buffer;
    return buffer;
}

qint64 BinaryReader::readInt64()
{
    if (!m_baseDevice->isReadable())
            throw QString("Unable to read from device!");

    qint64 buffer;
    m_dataStream >> buffer;
    return buffer;
}

qint8 BinaryReader::readSByte()
{
    if (!m_baseDevice->isReadable())
            throw QString("Unable to read from device!");

    qint8 buffer;
    m_dataStream >> buffer;
    return buffer;
}

float BinaryReader::readSingle()
{
    return readFloat();
}

QString BinaryReader::readString()
{
    if (!m_baseDevice->isReadable())
            throw QString("Unable to read from device!");

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
        n = m_dataStream.readRawData(qba.data()+numRead, byteCount);
        if (n == 0)
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
    if (!m_baseDevice->isReadable())
            throw QString("Unable to read from device!");

    quint16 buffer;
    m_dataStream >> buffer;
    return buffer;
}

quint32 BinaryReader::readUInt32()
{
    if (!m_baseDevice->isReadable())
            throw QString("Unable to read from device!");

    quint32 buffer;
    m_dataStream >> buffer;
    return buffer;
}

quint64 BinaryReader::readUInt64()
{
    if (!m_baseDevice->isReadable())
            throw QString("Unable to read from device!");

    quint64 buffer;
    m_dataStream >> buffer;
    return buffer;
}

} // namespace io
} // namespace qkeeg
