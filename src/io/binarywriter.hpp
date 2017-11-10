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
    /// Constructors
    BinaryWriter(QIODevice &writeDevice, const QSysInfo::Endian &endian = QSysInfo::ByteOrder);
    BinaryWriter(QIODevice &writeDevice, QTextCodec *codec, const QSysInfo::Endian &endian = QSysInfo::ByteOrder);

    /// Getters and Setters
    QIODevice *baseDevice();
    void setBaseDevice(QIODevice *baseDevice);

    QTextCodec *codec() const;
    void setCodec(QTextCodec *codec);

    QSysInfo::Endian endian() const;
    void setEndian(const QSysInfo::Endian &endian);

    /// Write functions

    virtual void write(const bool &value);
    virtual void write(const double &value);
    virtual void write(const float &value);

    virtual void write(const QByteArray &value, const qint32 &index, const qint32 &count);
    virtual void write(const QByteArray &value);
    virtual void write(const QVector<quint8> &value, const qint32 &index, const qint32 &count);
    virtual void write(const QVector<quint8> &value);

    virtual void write(const qint8 &value);
    virtual void write(const qint16 &value);
    virtual void write(const qint32 &value);
    virtual void write(const qint64 &value);

    virtual void write(const QString &value);

    virtual void write(const quint8 &value);
    virtual void write(const quint16 &value);
    virtual void write(const quint32 &value);
    virtual void write(const quint64 &value);

    /// Writes out a 32-bit integer in compressed format.
    virtual void write7BitEncodedInt(const qint32 &value);

    virtual void writeBString(const QString &value);
    virtual void writeBZString(const QString &value);
    virtual void writeWString(const QString &value);
    virtual void writeWZString(const QString &value);
    virtual void writeZString(const QString &value);

protected:
    const QByteArray m_defaultEncoding{"UTF-8"};
    QIODevice*       m_baseDevice;
    QTextCodec*      m_codec;
    QSysInfo::Endian m_endian;
    QDataStream      m_dataStream;
    QMutex           m_writeMutex;
};

} // namespace io
} // namespace qkeeg

#endif // BINARYWRITER_HPP
