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
// Data128FixedPoint is the base class for objects which are basically a 128 bit integer, with an 8 bit exponent
//  for fixed point integer computations
//
#ifndef DATA128FIXEDPOINT_H
#define DATA128FIXEDPOINT_H

#include "dataitem.h"

class Data128FixedPoint : public DataItem
{
    Q_OBJECT
public:
    explicit  Data128FixedPoint( typeCode_t tc = AO_UNDEFINED_DATAITEM, __int128 d = 0, qint8 ex = 0, QObject *p = NULL )
                : DataItem( tc, p ), v( d ), e( ex ) {}
              Data128FixedPoint( const Data128FixedPoint &d, QObject *p = NULL )
                : DataItem( d.typeCode, p ? p : d.parent() ), v( d.v ), e( d.e ) {}
              Data128FixedPoint( const QByteArray &di, QObject *p = NULL );
  QByteArray  toDataItem( bool cf = false ) const;
virtual void  operator = ( const QByteArray &di );
        void  operator = ( const  Data128FixedPoint &d ) { v = d.v; e = d.e; typeCode = d.typeCode; csVal = d.csVal; }
        void  operator = ( const __int128 &d ) { v = d; e = 0; }

protected:
    __int128  v; // generic value
       qint8  e; // exponent - like for a weird floating point number
};

#endif // DATA128FIXEDPOINT_H
