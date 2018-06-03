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
#ifndef DATAITEMBA_H
#define DATAITEMBA_H

#include <QByteArray>

/**
 * @brief The DataItemBA class - just a QByteArray that contains
 *   a serialized DataItem.  Used to distinguish between generic
 *   byte arrays and serialized data items.  Serialized data items
 *   take the form of:
 *   typeCode: 1 or more bytes
 *   if typeCode has a variable size, then size: 1 or more bytes
 *   data corresponding with the typeCode
 *
 *   The one or more bytes code:
 *     Values 0-127 are contained in a single byte, with MSbit = 0.
 *     If the first byte MSbit is 1, then the following byte contains
 *     the next 7 most significant bits - same scheme, if MSbit == 0
 *     then this is the end, otherwise the following byte contsins
 *     the next 7 most significant bits, rinse, lather, repeat.
 *
 *   For purposes of typeCode_t and sizes, a max of 4 bytes are allowed
 *     so, 2^28 or a range of 0 through 268,435,455
 *
 *   Considering moving those functions out of DataItem into here, but...
 *     they are more conveniently included in the project and accessed
 *     via DataItem.
 */
class DataItemBA : public QByteArray
{
public:
      explicit DataItemBA() : QByteArray() {}
               DataItemBA( const QByteArray &ba ) : QByteArray( ba ) {}

};

#endif // DATAITEMBA_H
