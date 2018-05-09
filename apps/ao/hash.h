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
// Assign Onward
//
// Part of the "Quantum Ready" structural demonstration, multiple
// accepted and handled hash data types.  When a quantum-ready
// hash algorithm is selected, add it to the lists.
//
#ifndef HASH_H
#define HASH_H

#include "hash256.h"
#include "hash512.h"

class Hash : public QObject
{
    Q_OBJECT
public:
       explicit  Hash( unsigned char tc = AO_HASH256, QObject *p = nullptr );
                 Hash( const QByteArray &di, QObject *p = nullptr );
                 Hash( const Hash &d, QObject *p = nullptr ) : QObject( p ? p : d.parent() ),
                   typeCode( d.typeCode ), hash256( d.hash256 ), hash512( d.hash512 ) {}
           Hash &calculate( QByteArray text );
           bool  verify( QByteArray text );
           bool  isValid();
           bool  isVerified();
     QByteArray  toDataItem();

private:
  unsigned char  typeCode;
        Hash256  hash256;
        Hash512  hash512;
};

#endif // HASH_H
