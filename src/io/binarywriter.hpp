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
#include <QIODevice>
#include <QObject>
#include <QMutex>
#include <QString>
#include <QTextCodec>
#include <QVector>
#include <common/macrohelpers.hpp>

namespace qkeeg { namespace io {

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

    BinaryWriter::Status status() const;

    /// Write functions

    virtual qint64 write(bool value);
    virtual qint64 write(double value);
    virtual qint64 write(float value);

    virtual qint64 write(const QByteArray &value, const qint32 &index, const qint32 &count);
    virtual qint64 write(const QByteArray &value);
    virtual qint64 write(const QVector<quint8> &value, const qint32 &index, const qint32 &count);
    virtual qint64 write(const QVector<quint8> &value);

    virtual qint64 write(qint8 value);
    virtual qint64 write(qint16 value);
    virtual qint64 write(qint32 value);
    virtual qint64 write(qint64 value);

    virtual qint64 write(const QString &value);

    virtual qint64 write(quint8 value);
    virtual qint64 write(quint16 value);
    virtual qint64 write(quint32 value);
    virtual qint64 write(quint64 value);

    /// Writes out a 32-bit integer in compressed format.
    virtual qint64 write7BitEncodedInt(qint32 value);

    virtual qint64 writeBString(const QString &value);
    virtual qint64 writeBZString(const QString &value);
    virtual qint64 writeWString(const QString &value);
    virtual qint64 writeWZString(const QString &value);
    virtual qint64 writeZString(const QString &value);

protected:
    const QByteArray m_defaultEncoding{"UTF-8"};
    QIODevice*       m_baseDevice;
    QTextCodec*      m_codec;
    QSysInfo::Endian m_byteOrder;
    QMutex           m_writeMutex;
    bool             m_doswap;
    Status           m_status;

    qint64 writeBlock(const void *buffer, const qint64 &index, const qint64 &count);
};

} // namespace io
} // namespace qkeeg

#endif // BINARYWRITER_HPP
