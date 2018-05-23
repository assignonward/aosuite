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
#ifndef BYTEARRAYSHORT_H
#define BYTEARRAYSHORT_H

#include "datavarlenshort.h"

class ByteArrayShort : public DataVarLenShort
{
    Q_OBJECT
public:
    explicit  ByteArrayShort( QByteArray ba = QByteArray(), QObject *p = NULL )
                : DataVarLenShort( AO_SHORTBYTEARRAY, ba, p ) {}
              ByteArrayShort( const ByteArrayShort &n, QObject *p = NULL )
                : DataVarLenShort( AO_SHORTBYTEARRAY, n.ba, p ? p : n.parent() ) {}
      qint32  size() { return ba.size(); }
        void  operator = ( const QByteArray &di ) { DataVarLenShort::operator = ( di ); }
        bool  operator < ( const ByteArrayShort &c ) const { return ba < c.ba; }
};

#endif // BYTEARRAYSHORT_H
