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
#ifndef BINARYREADER_HPP
#define BINARYREADER_HPP

#include <QByteArray>
#include <QObject>
#include <QDataStream>
#include <QTextCodec>
#include <QTextStream>
#include <QVector>

namespace qkeeg { namespace io {

#ifndef QDATASTREAM_DEFAULT_VERSION
    #define QDATASTREAM_DEFAULT_VERSION QDataStream::Qt_5_9
#endif

class BinaryReader : public QObject
{
    Q_OBJECT

public:
    /// Constructors
    BinaryReader(QIODevice &readDevice, const QSysInfo::Endian &endian = QSysInfo::ByteOrder);
    BinaryReader(QIODevice &readDevice, QTextCodec *codec, const QSysInfo::Endian &endian = QSysInfo::ByteOrder);

    /// Getters and Setters
    QTextCodec *codec() const;
    void setCodec(QTextCodec *codec);

    QIODevice *baseDevice();
    void setBaseDevice(QIODevice *baseDevice);

    QSysInfo::Endian endian() const;
    void setEndian(const QSysInfo::Endian &endian);

    /// Read functions

    /// Reads the specified number of bytes from the stream, starting from a specified point in the byte array.
    virtual qint32 read(QByteArray &buffer, const qint32 &index, const qint32 &count);
    virtual qint32 read(QVector<quint8> &buffer, const qint32 &index, const qint32 &count);

    /// Reads in a 32-bit integer in compressed format.
    virtual qint32 read7BitEncodedInt();

    /// Reads a Boolean value from the current device and advances the current position of the device by sizeof(bool).
    virtual bool readBoolean();
    /// Reads the next byte from the current device and advances the current position of the device by one byte.
    virtual quint8 readByte();
    /// Reads the specified number of bytes from the current device into a byte array and advances the current
    /// position by that number of bytes.
    virtual QByteArray readBytes(const qint32 &count);

    /// Reads the next character from the current device and advances the current position of the device in
    /// accordance with the Encoding used and the specific character being read from the device.
    //virtual QChar readChar();
    //virtual QVector<QChar> readChars(const qint32 &count);

    virtual double readDouble();
    virtual float readFloat();

    virtual qint16 readInt16();
    virtual qint32 readInt32();
    virtual qint64 readInt64();

    virtual qint8 readSByte();

    virtual float readSingle();

    /// Reads a string from the current device. The string is prefixed with the length,
    /// encoded as an integer seven bits at a time.
    virtual QString readString();

    virtual quint16 readUInt16();
    virtual quint32 readUInt32();
    virtual quint64 readUInt64();

protected:
    const QByteArray m_defaultEncoding{"UTF-8"};
    QIODevice*       m_baseDevice;
    QTextCodec*      m_codec;
    QSysInfo::Endian m_endian;
    QDataStream      m_dataStream;
};

} // namespace io
} // namespace qkeeg

#endif // BINARYREADER_HPP
