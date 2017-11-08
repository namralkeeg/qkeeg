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
#ifndef BINARYREADERS_HPP
#define BINARYREADERS_HPP

#include <common/endian.hpp>
#include <QIODevice>
#include <QDebug>
#include <QString>
#include <QTextCodec>

namespace qkeeg { namespace io {

/// Template helper for reading simple integer types. Is endian aware for integer types.
/// Defaults to system native byte order.
template<typename T>
qint64 readIntType(QIODevice &instream, T &data, const QSysInfo::Endian &endian = QSysInfo::Endian::ByteOrder)
{
    static_assert(std::is_integral<T>::value
                  && !std::is_same<T, bool>::value, "T must be an integer type!");
    try
    {
        if (instream.isReadable())
        {
            qint64 bytesRead = instream.read(reinterpret_cast<char*>(&data), sizeof(T));
            if (bytesRead > 0)
            {
                convertToEndianInplace<T>(data, endian);
                return static_cast<quint64>(bytesRead);
            }
        }
    }
    catch(...)
    {
        //qDebug() << ex.what() << std::endl;
        return -1;
    }

    return -1;
}

///// Template helper for reading simple POD types. Usually used for simple structs.
template<typename T>
qint64 readPODType(QIODevice &instream, T &data)
{
    static_assert(std::is_pod<T>::value && std::is_trivially_copyable<T>::value, "T must be a POD!");
    try
    {
        if (instream.isReadable())
        {
            qint64 bytesRead = instream.read(reinterpret_cast<char*>(&data), sizeof(T));
            return static_cast<quint64>(bytesRead);
        }
    }
    catch(...)
    {
        //qDebug() << ex.what() << std::endl;
        return -1;
    }

    return -1;
}

template<typename T>
qint64 readPrefixString(QIODevice &instream, QString &data,
                        QTextCodec *codec = QTextCodec::codecForName("UTF-8"),
                        const QSysInfo::Endian &endian = QSysInfo::Endian::ByteOrder,
                        const bool &isNullTerminated = false)
{
    static_assert(std::is_integral<T>::value &&
                  !std::is_same<T, bool>::value, "T must be any integer type!");
    try
    {
        if (instream.isReadable())
        {
            T size;
            if(readIntType<T>(instream, size, endian) > 0)
            {
                QByteArray qba(size, char(0));
                qint64 bytesRead = instream.read(qba.data(), size);

                if(isNullTerminated)
                {
                    // Removes the ending null.
                    qba.resize(size-1);
                }

                if (codec == nullptr)
                    codec = QTextCodec::codecForName("UTF-8");
                data = QString(codec->toUnicode(qba));

                return static_cast<qint64>(bytesRead + sizeof(T));
            }
        }
    }
    catch(...)
    {
        //qDebug() << ex.what() << std::endl;
        return -1;
    }

    return -1;
}

/// Read convience helpers for the readIntType template.
qint64 readInt(QIODevice &instream, qint16 &data);
qint64 readInt(QIODevice &instream, qint32 &data);
qint64 readInt(QIODevice &instream, qint64 &data);

qint64 readUInt(QIODevice &instream, quint16 &data);
qint64 readUInt(QIODevice &instream, quint32 &data);
qint64 readUInt(QIODevice &instream, quint64 &data);

/// For bool types. bool isn't standardized between platforms at this time.
qint64 readBoolean(QIODevice &instream, bool &data,
                   const QSysInfo::Endian &endian = QSysInfo::Endian::ByteOrder);

/// Reads length bytes into data starting from index from the input stream.
/// Data is resized if it's smaller than length.
qint64 readBytes(QIODevice &instream, QByteArray &data, const qint32 &length, const qint32 &index);

/// Reads length bytes into data from the input stream. Data is resized if it's smaller than length.
qint64 readBytes(QIODevice &instream, QByteArray &data, const qint32 &length);

/// Read a string prefixed with a quint8 length. NOT zero terminated.
qint64 readBString(QIODevice &instream, QString &data, QTextCodec *codec = QTextCodec::codecForName("UTF-8"),
                   const QSysInfo::Endian &endian = QSysInfo::Endian::ByteOrder);

/// Read a string prefixed with a quint8 length. zero terminated.
qint64 readBZString(QIODevice &instream, QString &data, QTextCodec *codec = QTextCodec::codecForName("UTF-8"),
                    const QSysInfo::Endian &endian = QSysInfo::Endian::ByteOrder);

/// Read a string prefixed with a uint16 length. NOT zero terminated.
qint64 readWString(QIODevice &instream, QString &data, QTextCodec *codec = QTextCodec::codecForName("UTF-8"),
                   const QSysInfo::Endian &endian = QSysInfo::Endian::ByteOrder);

/// Read a string prefixed with a uint16 length. zero terminated.
qint64 readWZString(QIODevice &instream, QString &data, QTextCodec *codec = QTextCodec::codecForName("UTF-8"),
                   const QSysInfo::Endian &endian = QSysInfo::Endian::ByteOrder);

/// Zero terminated string.
/// Size is size of string text + 1 for string terminator.
qint64 readZString(QIODevice &instream, QString &data, QTextCodec *codec = QTextCodec::codecForName("UTF-8"));

} // namespace io
} // namespace qkeeg

#endif // BINARYREADERS_HPP
