/* MIT License
 *
 * Copyright (c) 2018 Assign Onward
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef VARSIZECODE_H
#define VARSIZECODE_H

#include <QByteArray>

/**
 * @brief The VarSizeCode class contains functions to encode and decode unsigned integers as
 *   a variable length byte array with 7 bits of "value" in each byte, if the byte
 *   has a 1 in the MSb, then the next byte contains the next 7 more significant bits -
 *   continue until a byte with a 0 in the MSb is found.  Default functions work with 32 bit
 *   unsigned integers, but the same scheme could be applied to 64 or even 128 bit integers,
 *   and signed integers might be accomodated with a double encoding scheme that essentially
 *   puts the sign bit in the LSb, similar to negative value Golomb-Rice codes.
 */
class VarSizeCode
{
public:
                   VarSizeCode() {}
static    quint32  bytesToCode( const QByteArray &sa, qint32 &i );
static QByteArray  codeToBytes( const quint32 &s );
};

#endif // VARSIZECODE_H
