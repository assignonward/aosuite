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
#include "hash224salt32.h"

class Hash : public DataItem
{
    Q_OBJECT
public:
    explicit  Hash( typeCode_t tc = AO_HASH256, QObject *p = NULL );
              Hash( const QByteArray &di, QObject *p = NULL );
              Hash( const Hash &d, QObject *p = NULL )
                : DataItem( d.typeCode, p ? p : d.parent() ),
                  hash256( d.hash256 ),
                  hash512( d.hash512 ),
                  hash224salt32( d.hash224salt32 ) {}
        void  operator = ( const Hash &h )
                { typeCode = h.typeCode;
                  hash256 = h.hash256;
                  hash512 = h.hash512;
                  hash224salt32 = h.hash224salt32; }
  QByteArray  toDataItem( bool cf = false ) const;
        Hash &calculate( QByteArray text );
        bool  verify( QByteArray text );
        bool  isValid();
        bool  isVerified();

private:
     Hash256  hash256;
     Hash512  hash512;
     Hash224Salt32  hash224salt32;
};

#endif // HASH_H
