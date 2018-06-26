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
// DataVarInt32 is the base class for objects which are basically a 32 bit integer, but stored as
//   a variable byte code 1-5 bytes long.
//
#ifndef DATAVARINT32_H
#define DATAVARINT32_H

#include "data32.h"

class DataVarInt32 : public Data32
{
    Q_OBJECT
public:
    explicit  DataVarInt32( qint32 d = 0, typeCode_t tc = AO_UNDEFINED_DATAITEM, QObject *p = NULL )
                : Data32( d, tc, p ) {}
              DataVarInt32( const DataVarInt32 &d, QObject *p = NULL )
                : Data32( d.v, d.typeCode, p ? p : d.parent() ) {}
              DataVarInt32( const DataItemBA &di, QObject *p = NULL );
  DataItemBA  toDataItem( bool cf = false ) const;
virtual void  operator = ( const DataItemBA &di );
        void  operator = ( const qint32 &d ) { v = d; }
};

#endif // DATAVARINT32_H
