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
#include <array>

namespace qkeeg { namespace io {

BinaryWriter::BinaryWriter(QIODevice &writeDevice, const QSysInfo::Endian &byteOrder) :
    m_baseDevice(&writeDevice), m_codec(QTextCodec::codecForName(m_defaultEncoding)),
    m_byteOrder(byteOrder)
{
    m_doswap = (QSysInfo::ByteOrder != m_byteOrder) ? true : false;
}

BinaryWriter::BinaryWriter(QIODevice &writeDevice, QTextCodec *codec, const QSysInfo::Endian &byteOrder) :
    m_baseDevice(&writeDevice), m_codec(codec), m_byteOrder(byteOrder)
{
    m_doswap = (QSysInfo::ByteOrder != m_byteOrder) ? true : false;
    if (m_codec == nullptr) {
        m_codec = QTextCodec::codecForName(m_defaultEncoding);
    }
}

QIODevice *BinaryWriter::baseDevice()
{
    return m_baseDevice;
}

void BinaryWriter::setBaseDevice(QIODevice *baseDevice)
{
    m_baseDevice = baseDevice;
}

QTextCodec *BinaryWriter::codec() const
{
    return m_codec;
}

void BinaryWriter::setCodec(QTextCodec *codec)
{
    if (codec != nullptr) {
        m_codec = codec;
    }
}

QSysInfo::Endian BinaryWriter::endian() const
{
    return m_byteOrder;
}

void BinaryWriter::setEndian(const QSysInfo::Endian &endian)
{
    m_byteOrder = endian;
    m_doswap = (QSysInfo::ByteOrder != m_byteOrder) ? true : false;
}

BinaryWriter::Status BinaryWriter::status() const
{
    return m_status;
}

qint64 BinaryWriter::write(bool value)
{
    quint8 buffer = (value) ? 1 : 0;
    return write(buffer);
}

qint64 BinaryWriter::write(double value)
{
    std::array<quint8, sizeof(value)> buffer;
    quint64 temp;
    std::memcpy(&temp, &value, sizeof(value));

    if (m_byteOrder == QSysInfo::Endian::LittleEndian)
    {
        buffer[0] = static_cast<quint8>(temp & 0xFF);
        buffer[1] = static_cast<quint8>((temp >> 8) & 0xFF);
        buffer[2] = static_cast<quint8>((temp >> 16) & 0xFF);
        buffer[3] = static_cast<quint8>((temp >> 24) & 0xFF);
        buffer[4] = static_cast<quint8>((temp >> 32) & 0xFF);
        buffer[5] = static_cast<quint8>((temp >> 40) & 0xFF);
        buffer[6] = static_cast<quint8>((temp >> 48) & 0xFF);
        buffer[7] = static_cast<quint8>((temp >> 56) & 0xFF);
    }
    else
    {
        buffer[0] = static_cast<quint8>((temp >> 56) & 0xFF);
        buffer[1] = static_cast<quint8>((temp >> 48) & 0xFF);
        buffer[2] = static_cast<quint8>((temp >> 40) & 0xFF);
        buffer[3] = static_cast<quint8>((temp >> 32) & 0xFF);
        buffer[4] = static_cast<quint8>((temp >> 24) & 0xFF);
        buffer[5] = static_cast<quint8>((temp >> 16) & 0xFF);
        buffer[6] = static_cast<quint8>((temp >> 8) & 0xFF);
        buffer[7] = static_cast<quint8>(temp & 0xFF);
    }

    return writeBlock(buffer.data(), 0, sizeof(temp));

//    qint64 bytesWritten = -1;

//    if (m_doswap)
//    {
//        union
//        {
//            double val1;
//            quint64 val2;
//        } x;

//        x.val1 = value;
//        x.val2 = common::swap<quint64>(x.val2);

//        bytesWritten = writeBlock(&x.val2, 0, sizeof(quint64));
//    }
//    else
//    {
//        bytesWritten = writeBlock(&value, 0, sizeof(double));
//    }

//    return bytesWritten;
}

qint64 BinaryWriter::write(float value)
{
//    qint64 bytesWritten = -1;
    std::array<quint8, sizeof(value)> buffer;
    quint32 temp;
    std::memcpy(&temp, &value, sizeof(value));

    if (m_byteOrder == QSysInfo::Endian::LittleEndian)
    {
        buffer[0] = static_cast<quint8>(temp & 0xFF);
        buffer[1] = static_cast<quint8>((temp >> 8) & 0xFF);
        buffer[2] = static_cast<quint8>((temp >> 16) & 0xFF);
        buffer[3] = static_cast<quint8>((temp >> 24) & 0xFF);
    }
    else
    {
        buffer[0] = static_cast<quint8>((temp >> 24) & 0xFF);
        buffer[1] = static_cast<quint8>((temp >> 16) & 0xFF);
        buffer[2] = static_cast<quint8>((temp >> 8) & 0xFF);
        buffer[3] = static_cast<quint8>(temp & 0xFF);
    }

    return writeBlock(buffer.data(), 0, sizeof(temp));
//    return writeBlock(buffer.data(), 0, sizeof(float));
//    if (m_doswap)
//    {
//        union
//        {
//            float val1;
//            quint32 val2;
//        } x;

//        x.val1 = value;
//        x.val2 = common::swap<quint32>(x.val2);

//        bytesWritten = writeBlock(&x.val2, 0, sizeof(quint32));
//    }
//    else
//    {
//        bytesWritten = writeBlock(&value, 0, sizeof(float));
//    }

//    return bytesWritten;
}

qint64 BinaryWriter::write(const QByteArray &value, const qint32 &index, const qint32 &count)
{
    return writeBlock(value.data(), index, count);
}

qint64 BinaryWriter::write(const QByteArray &value)
{
    return write(value, 0, value.size());
}

qint64 BinaryWriter::write(const QVector<quint8> &value, const qint32 &index, const qint32 &count)
{
    return writeBlock(value.data(), index, count);
}

qint64 BinaryWriter::write(const QVector<quint8> &value)
{
    return write(value, 0, value.size());
}

qint64 BinaryWriter::write(qint8 value)
{
    return writeBlock(&value, 0, sizeof(value));
}

qint64 BinaryWriter::write(qint16 value)
{
    qint16 buffer = (m_doswap) ? common::swap<qint16>(value) : value;

    return writeBlock(&buffer, 0, sizeof(buffer));
}

qint64 BinaryWriter::write(qint32 value)
{
    qint32 buffer = (m_doswap) ? common::swap<qint32>(value) : value;

    return writeBlock(&buffer, 0, sizeof(buffer));
}

qint64 BinaryWriter::write(qint64 value)
{
    qint64 buffer = (m_doswap) ? common::swap<qint64>(value) : value;

    return writeBlock(&buffer, 0, sizeof(buffer));
}

qint64 BinaryWriter::write(const QString &value)
{
    if (!m_codec->canEncode(value)) {
        throw QString("Unable to convert string to specified encoding!");
    }

    qint64 bytesWritten = 0;
    if (value.size() > 0)
    {
        QByteArray qba = m_codec->fromUnicode(value);
        bytesWritten += write7BitEncodedInt(qba.size());
        bytesWritten += write(qba);
    }

    return bytesWritten;
}

qint64 BinaryWriter::write(quint8 value)
{
    return writeBlock(&value, 0, sizeof(value));
}

qint64 BinaryWriter::write(quint16 value)
{
    quint16 buffer = (m_doswap) ? common::swap<quint16>(value) : value;

    return writeBlock(&buffer, 0, sizeof(buffer));
}

qint64 BinaryWriter::write(quint32 value)
{
    quint32 buffer = (m_doswap) ? common::swap<quint32>(value) : value;

    return writeBlock(&buffer, 0, sizeof(buffer));
}

qint64 BinaryWriter::write(quint64 value)
{
    quint64 buffer = (m_doswap) ? common::swap<quint64>(value) : value;

    return writeBlock(&buffer, 0, sizeof(buffer));
}

qint64 BinaryWriter::write7BitEncodedInt(qint32 value)
{
    // Number is processed Little-Endian
    quint32 v = common::native_to_little<quint32>(static_cast<quint32>(value)); // support negative numbers
    qint64 bytesWritten = 0;
    while (v >= 0x80)
    {
        bytesWritten += write(static_cast<quint8>(v | 0x80));
        v >>= 7;
    }

    bytesWritten += write(static_cast<quint8>(v));

    return bytesWritten;
}

qint64 BinaryWriter::writeBString(const QString &value)
{
    if (!m_codec->canEncode(value))
        throw QString("Unable to convert string to specified encoding!");

    QByteArray qba = m_codec->fromUnicode(value);
    if (qba.size() > 0xFF)
        qba.resize(0xFF);
    quint8 size = static_cast<quint8>(qba.size());

    qint64 bytesWritten = 0;
    bytesWritten += write(size);
    bytesWritten += write(qba);

    return bytesWritten;
}

qint64 BinaryWriter::writeBZString(const QString &value)
{
    if (!m_codec->canEncode(value))
        throw QString("Unable to convert string to specified encoding!");

    QByteArray qba = m_codec->fromUnicode(value);
    if (qba.size() >= 0xFF) {
        qba.resize(0xFE);
    }

    // Append null to the string.
    qba += char(0);
    quint8 size = static_cast<quint8>(qba.size());

    qint64 bytesWritten = 0;
    bytesWritten += write(size);
    bytesWritten += write(qba);

    return bytesWritten;
}

qint64 BinaryWriter::writeWString(const QString &value)
{
    if (!m_codec->canEncode(value)) {
        throw QString("Unable to convert string to specified encoding!");
    }

    QByteArray qba = m_codec->fromUnicode(value);
    if (qba.size() > 0xFFFF)
        qba.resize(0xFFFF);
    quint16 size = static_cast<quint16>(qba.size());

    qint64 bytesWritten = 0;
    bytesWritten += write(size);
    bytesWritten += write(qba);

    return bytesWritten;
}

qint64 BinaryWriter::writeWZString(const QString &value)
{
    if (!m_codec->canEncode(value)) {
        throw QString("Unable to convert string to specified encoding!");
    }

    QByteArray qba = m_codec->fromUnicode(value);
    if (qba.size() >= 0xFFFF)
        qba.resize(0xFFFE);

    // Append null to the string.
    qba += char(0);
    quint16 size = static_cast<quint16>(qba.size());

    qint64 bytesWritten = 0;
    bytesWritten += write(size);
    bytesWritten += write(qba);

    return bytesWritten;
}

qint64 BinaryWriter::writeZString(const QString &value)
{
    if (!m_codec->canEncode(value)) {
        throw QString("Unable to convert string to specified encoding!");
    }

    QByteArray qba = m_codec->fromUnicode(value);
    // Append null to the string.
    qba += char(0);

    return write(qba);
}

qint64 BinaryWriter::writeBlock(const void *buffer, const qint64 &index, const qint64 &count)
{
    QMutexLocker lock(&m_writeMutex);
    qint64 bytesWritten = -1;

    if (!m_baseDevice->isWritable()) {
        return -1;
    }

    try
    {
        if (index < 0 || count < 0) {
            return -1;
        }
        else if (count == 0) {
            return 0;
        }

        const char *current = reinterpret_cast<const char *>(buffer) + index;

        bytesWritten = m_baseDevice->write(current, count);

        if (bytesWritten != count) {
            m_status = WriteFailed;
        }

    }
    catch(...)
    {
        return bytesWritten;
    }

    return bytesWritten;
}

} // namespace io
} // namespace qkeeg
