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
#include <io/binaryreaders.hpp>

namespace qkeeg { namespace io {

qint64 readBoolean(QIODevice &instream, bool &data, const QSysInfo::Endian &endian)
{
    try
    {
        if (instream.isReadable())
        {
            qint64 status = -1;

            switch (sizeof(bool)) {
            case sizeof(quint64):
            {
                quint64 buffer;
                status = readIntType<quint64>(instream, buffer, endian);
                if(status)
                    data = static_cast<bool>(buffer);
            }
                break;
            case sizeof(quint32):
            {
                quint32 buffer;
                status = readIntType<quint32>(instream, buffer, endian);
                if(status)
                    data = static_cast<bool>(buffer);
            }
                break;
            case sizeof(quint16):
            {
                quint16 buffer;
                status = readIntType<quint16>(instream, buffer, endian);
                if(status)
                    data = static_cast<bool>(buffer);
            }
                break;
            case sizeof(quint8):
            {
                quint8 buffer;
                status = readIntType<quint8>(instream, buffer, endian);
                if(status)
                    data = static_cast<bool>(buffer);
            }
                break;
            default:
                break;
            }

            return status;
        }
    }
    catch(const std::exception &ex)
    {
        //qDebug() << ex.what() << std::endl;
        return -1;
    }

    return -1;
}

qint64 readBytes(QIODevice &instream, QByteArray &data, const qint32 &length, const qint32 &index)
{
    try
    {
        if (length < 1 || index < 0)
            return -1;

        if (instream.isReadable())
        {
            if (length + index > data.size())
                data.resize(length + index);

            qint64 bytesRead = instream.read(data.data() + index, length);

            if ((bytesRead < length) && (bytesRead != -1))
                data.resize(bytesRead);

            return bytesRead;
        }
    }
    catch(...)
    {
        //std::cerr << ex.what() << std::endl;
        return -1;
    }

    return -1;
}

qint64 readBytes(QIODevice &instream, QByteArray &data, const qint32 &length)
{
    return readBytes(instream, data, length, 0);
}

qint64 readBString(QIODevice &instream, QString &data, QTextCodec *codec, const QSysInfo::Endian &endian)
{
    return readPrefixString<quint8>(instream, data, codec, endian, false);
}

qint64 readBZString(QIODevice &instream, QString &data, QTextCodec *codec, const QSysInfo::Endian &endian)
{
    return readPrefixString<quint8>(instream, data, codec, endian, true);
}

qint64 readWString(QIODevice &instream, QString &data, QTextCodec *codec, const QSysInfo::Endian &endian)
{
    return readPrefixString<quint16>(instream, data, codec, endian, false);
}

qint64 readWZString(QIODevice &instream, QString &data, QTextCodec *codec, const QSysInfo::Endian &endian)
{
    return readPrefixString<quint16>(instream, data, codec, endian, true);
}

qint64 readZString(QIODevice &instream, QString &data, QTextCodec *codec)
{
    try
    {
        if (instream.isReadable())
        {
            QByteArray qba;
            char c;
            qint64 bytesRead = 0;

            while (instream.read(&c, sizeof(char)) > 0)
            {
                if (c != '\0')
                    qba += c;
                else
                    break;

                bytesRead++;
            }

            if (codec == nullptr)
                codec = QTextCodec::codecForName("UTF-8");
            data = QString(codec->toUnicode(qba));

            return bytesRead;
        }
    }
    catch(...)
    {
        //qDebug() << ex.what() << std::endl;
        return -1;
    }

    return -1;
}

qint64 readInt(QIODevice &instream, qint16 &data)
{
    return readIntType(instream, data);
}

qint64 readInt(QIODevice &instream, qint32 &data)
{
    return readIntType(instream, data);
}

qint64 readInt(QIODevice &instream, qint64 &data)
{
    return readIntType(instream, data);
}

qint64 readUInt(QIODevice &instream, quint16 &data)
{
    return readIntType(instream, data);
}

qint64 readUInt(QIODevice &instream, quint32 &data)
{
    return readIntType(instream, data);
}

qint64 readUInt(QIODevice &instream, quint64 &data)
{
    return readIntType(instream, data);
}

} // namespace io
} // namespace qkeeg
