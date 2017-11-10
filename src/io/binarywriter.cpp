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
#include <io/binarywriter.hpp>
#include <common/endian.hpp>

namespace qkeeg { namespace io {

BinaryWriter::BinaryWriter(QIODevice &writeDevice, const QSysInfo::Endian &endian) :
    m_baseDevice(&writeDevice), m_codec(QTextCodec::codecForName(m_defaultEncoding)),
    m_endian(endian), m_dataStream(m_baseDevice)
{
    m_dataStream.setByteOrder(static_cast<QDataStream::ByteOrder>(endian));
    m_dataStream.setVersion(QDATASTREAM_DEFAULT_VERSION);
}

BinaryWriter::BinaryWriter(QIODevice &writeDevice, QTextCodec *codec, const QSysInfo::Endian &endian) :
    m_baseDevice(&writeDevice), m_codec(codec), m_endian(endian), m_dataStream(m_baseDevice)
{
    m_dataStream.setByteOrder(static_cast<QDataStream::ByteOrder>(endian));
    m_dataStream.setVersion(QDATASTREAM_DEFAULT_VERSION);
}

QIODevice *BinaryWriter::baseDevice()
{
    return m_baseDevice;
}

void BinaryWriter::setBaseDevice(QIODevice *baseDevice)
{
    m_baseDevice = baseDevice;
    m_dataStream.setDevice(m_baseDevice);
}

QTextCodec *BinaryWriter::codec() const
{
    return m_codec;
}

void BinaryWriter::setCodec(QTextCodec *codec)
{
    if (codec != nullptr)
    {
        m_codec = codec;
    }
}

QSysInfo::Endian BinaryWriter::endian() const
{
    return m_endian;
}

void BinaryWriter::setEndian(const QSysInfo::Endian &endian)
{
    m_endian = endian;
    m_dataStream.setByteOrder(static_cast<QDataStream::ByteOrder>(endian));
}

void BinaryWriter::write(const bool &value)
{
    QMutexLocker lock(&m_writeMutex);

    if (!m_baseDevice->isWritable())
        throw QString("Unable to write to device.");

    quint8 buffer = (value) ? 1 : 0;
    m_dataStream << buffer;
}

void BinaryWriter::write(const double &value)
{
    QMutexLocker lock(&m_writeMutex);

    if (!m_baseDevice->isWritable())
        throw QString("Unable to write to device.");

    m_dataStream.setFloatingPointPrecision(QDataStream::DoublePrecision);
    m_dataStream << value;
}

void BinaryWriter::write(const float &value)
{
    QMutexLocker lock(&m_writeMutex);

    if (!m_baseDevice->isWritable())
        throw QString("Unable to write to device.");

    m_dataStream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    m_dataStream << value;
}

void BinaryWriter::write(const QByteArray &value, const qint32 &index, const qint32 &count)
{
    QMutexLocker lock(&m_writeMutex);

    if (!m_baseDevice->isWritable())
        throw QString("Unable to write to device.");

    m_dataStream.writeRawData(value.data()+index, count);
}

void BinaryWriter::write(const QByteArray &value)
{
    write(value, 0, value.size());
}

void BinaryWriter::write(const QVector<quint8> &value, const qint32 &index, const qint32 &count)
{
    QMutexLocker lock(&m_writeMutex);

    if (!m_baseDevice->isWritable())
        throw QString("Unable to write to device.");

    m_dataStream.writeRawData(reinterpret_cast<const char*>(value.data()+index), count);
}

void BinaryWriter::write(const QVector<quint8> &value)
{
    write(value, 0, value.size());
}

void BinaryWriter::write(const qint8 &value)
{
    QMutexLocker lock(&m_writeMutex);

    if (!m_baseDevice->isWritable())
        throw QString("Unable to write to device.");

    m_dataStream << value;
}

void BinaryWriter::write(const qint16 &value)
{
    QMutexLocker lock(&m_writeMutex);

    if (!m_baseDevice->isWritable())
        throw QString("Unable to write to device.");

    m_dataStream << value;
}

void BinaryWriter::write(const qint32 &value)
{
    QMutexLocker lock(&m_writeMutex);

    if (!m_baseDevice->isWritable())
        throw QString("Unable to write to device.");

    m_dataStream << value;
}

void BinaryWriter::write(const qint64 &value)
{
    QMutexLocker lock(&m_writeMutex);

    if (!m_baseDevice->isWritable())
        throw QString("Unable to write to device.");

    m_dataStream << value;
}

void BinaryWriter::write(const QString &value)
{
    QMutexLocker lock(&m_writeMutex);

    if (!m_baseDevice->isWritable())
        throw QString("Unable to write to device.");

    if (!m_codec->canEncode(value))
        throw QString("Unable to convert string to specified encoding!");

    if (value.size() > 0)
    {
        QByteArray qba = m_codec->fromUnicode(value);
        write7BitEncodedInt(qba.size());
        write(qba);
    }

}

void BinaryWriter::write(const quint8 &value)
{
    QMutexLocker lock(&m_writeMutex);

    if (!m_baseDevice->isWritable())
        throw QString("Unable to write to device.");

    m_dataStream << value;
}

void BinaryWriter::write(const quint16 &value)
{
    QMutexLocker lock(&m_writeMutex);

    if (!m_baseDevice->isWritable())
        throw QString("Unable to write to device.");

    m_dataStream << value;
}

void BinaryWriter::write(const quint32 &value)
{
    QMutexLocker lock(&m_writeMutex);

    if (!m_baseDevice->isWritable())
        throw QString("Unable to write to device.");

    m_dataStream << value;
}

void BinaryWriter::write(const quint64 &value)
{
    QMutexLocker lock(&m_writeMutex);

    if (!m_baseDevice->isWritable())
        throw QString("Unable to write to device.");

    m_dataStream << value;
}

void BinaryWriter::write7BitEncodedInt(const qint32 &value)
{
    QMutexLocker lock(&m_writeMutex);

    if (!m_baseDevice->isWritable())
        throw QString("Unable to write to device.");

    // Number is processed Little-Endian
    quint32 v = static_cast<quint32>(native_to_little(value)); // support negative numbers
    while (v >= 0x80)
    {
        write(static_cast<quint8>(v | 0x80));
        v >>= 7;
    }

    write(static_cast<quint8>(v));
}

void BinaryWriter::writeBString(const QString &value)
{
    QMutexLocker lock(&m_writeMutex);

    if (!m_baseDevice->isWritable())
        throw QString("Unable to write to device.");

    if (!m_codec->canEncode(value))
        throw QString("Unable to convert string to specified encoding!");

    QByteArray qba = m_codec->fromUnicode(value);
    if (qba.size() > 0xFF)
        qba.resize(0xFF);
    quint8 size = static_cast<quint8>(qba.size());

    write(size);
    write(qba);
}

void BinaryWriter::writeBZString(const QString &value)
{
    QMutexLocker lock(&m_writeMutex);

    if (!m_baseDevice->isWritable())
        throw QString("Unable to write to device.");

    if (!m_codec->canEncode(value))
        throw QString("Unable to convert string to specified encoding!");

    QByteArray qba = m_codec->fromUnicode(value);
    if (qba.size() >= 0xFF)
        qba.resize(0xFE);

    // Append null to the string.
    qba += char(0);
    quint8 size = static_cast<quint8>(qba.size());

    write(size);
    write(qba);
}

void BinaryWriter::writeWString(const QString &value)
{
    QMutexLocker lock(&m_writeMutex);

    if (!m_baseDevice->isWritable())
        throw QString("Unable to write to device.");

    if (!m_codec->canEncode(value))
        throw QString("Unable to convert string to specified encoding!");

    QByteArray qba = m_codec->fromUnicode(value);
    if (qba.size() > 0xFFFF)
        qba.resize(0xFFFF);
    quint16 size = static_cast<quint16>(qba.size());

    write(size);
    write(qba);
}

void BinaryWriter::writeWZString(const QString &value)
{
    QMutexLocker lock(&m_writeMutex);

    if (!m_baseDevice->isWritable())
        throw QString("Unable to write to device.");

    if (!m_codec->canEncode(value))
        throw QString("Unable to convert string to specified encoding!");

    QByteArray qba = m_codec->fromUnicode(value);
    if (qba.size() >= 0xFFFF)
        qba.resize(0xFFFE);

    // Append null to the string.
    qba += char(0);
    quint16 size = static_cast<quint16>(qba.size());

    write(size);
    write(qba);
}

void BinaryWriter::writeZString(const QString &value)
{
    QMutexLocker lock(&m_writeMutex);

    if (!m_baseDevice->isWritable())
        throw QString("Unable to write to device.");

    if (!m_codec->canEncode(value))
        throw QString("Unable to convert string to specified encoding!");

    QByteArray qba = m_codec->fromUnicode(value);
    // Append null to the string.
    qba += char(0);

    write(qba);
}

} // namespace io
} // namespace qkeeg
