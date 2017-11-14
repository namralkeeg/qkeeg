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
#ifndef BINARYWRITER_HPP
#define BINARYWRITER_HPP

#include <QByteArray>
#include <QDataStream>
#include <QObject>
#include <QMutex>
#include <QString>
#include <QTextCodec>
#include <QVector>

namespace qkeeg { namespace io {

#ifndef QDATASTREAM_DEFAULT_VERSION
    #define QDATASTREAM_DEFAULT_VERSION QDataStream::Qt_5_9
#endif

class BinaryWriter : public QObject
{
    Q_OBJECT

public:

    enum Status
    {
        Ok,
        WriteFailed,
    };

    /// Constructors
    BinaryWriter(QIODevice &writeDevice, const QSysInfo::Endian &byteOrder = QSysInfo::ByteOrder);
    BinaryWriter(QIODevice &writeDevice, QTextCodec *codec, const QSysInfo::Endian &byteOrder = QSysInfo::ByteOrder);

    /// Getters and Setters
    QIODevice *baseDevice();
    void setBaseDevice(QIODevice *baseDevice);

    QTextCodec *codec() const;
    void setCodec(QTextCodec *codec);

    QSysInfo::Endian endian() const;
    void setEndian(const QSysInfo::Endian &endian);

    /// Write functions

    virtual qint32 write(const bool &value);
    virtual qint32 write(const double &value);
    virtual qint32 write(const float &value);

    virtual qint32 write(const QByteArray &value, const qint32 &index, const qint32 &count);
    virtual qint32 write(const QByteArray &value);
    virtual qint32 write(const QVector<quint8> &value, const qint32 &index, const qint32 &count);
    virtual qint32 write(const QVector<quint8> &value);

    virtual qint32 write(const qint8 &value);
    virtual qint32 write(const qint16 &value);
    virtual qint32 write(const qint32 &value);
    virtual qint32 write(const qint64 &value);

    virtual qint32 write(const QString &value);

    virtual qint32 write(const quint8 &value);
    virtual qint32 write(const quint16 &value);
    virtual qint32 write(const quint32 &value);
    virtual qint32 write(const quint64 &value);

    /// Writes out a 32-bit integer in compressed format.
    virtual qint32 write7BitEncodedInt(const qint32 &value);

    virtual qint32 writeBString(const QString &value);
    virtual qint32 writeBZString(const QString &value);
    virtual qint32 writeWString(const QString &value);
    virtual qint32 writeWZString(const QString &value);
    virtual qint32 writeZString(const QString &value);

protected:
    const QByteArray m_defaultEncoding{"UTF-8"};
    QIODevice*       m_baseDevice;
    QTextCodec*      m_codec;
    QSysInfo::Endian m_byteOrder;
    //QDataStream      m_dataStream;
    QMutex           m_writeMutex;
    bool             m_doswap;
    Status           m_status;

    qint64 writeBlock(const void *buffer, const qint64 &index, const qint64 &count);
};

} // namespace io
} // namespace qkeeg

#endif // BINARYWRITER_HPP
