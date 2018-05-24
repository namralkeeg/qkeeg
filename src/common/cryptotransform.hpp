/*
 * Copyright (C) 2018 Larry Lopez
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
#ifndef CRYPTOTRANSFORM_HPP
#define CRYPTOTRANSFORM_HPP

#include <QtGlobal>

class CryptoTransform
{
public:
    virtual ~CryptoTransform() {}

    virtual qint32 inputBlockSize() const = 0;
    virtual qint32 outputBlockSize() const = 0;
    virtual bool canTransformMultipleBlocks() const = 0;
    virtual bool canReuseTransform() const = 0;

    virtual qint32 transformBlock(const QByteArray &inputBuffer, qint32 inputOffset, qint32 inputCount,
                                  QByteArray &outputBuffer, qint32 outputOffset) = 0;
    virtual QByteArray transformFinalBlock(const QByteArray &inputBuffer, qint32 inputOffset, qint32 inputCount) = 0;

protected:
    CryptoTransform() {}
}

#endif // CRYPTOTRANSFORM_HPP
