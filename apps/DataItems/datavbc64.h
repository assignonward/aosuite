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
// DataVlbc64 is the base class for objects which are basically a 64 bit integer, but stored as
//   a variable byte code 1-9 bytes long.
//
#ifndef DATAVBC64_H
#define DATAVBC64_H

#include "data64.h"

/**
 * @brief The DataVbc64 class - 64 bit integer, stored as a variable byte code 1-9 bytes long
 */
class DataVbc64 : public Data64
{
    Q_OBJECT
public:
              DataVbc64( QObject *p = NULL )
                : Data64( 0, AO_UNDEFINED_DATAITEM, p ) {}
              DataVbc64( qint64 d, typeCode_t tc = AO_UNDEFINED_DATAITEM, QObject *p = NULL )
                : Data64( d, tc, p ) {}
              DataVbc64( const DataVbc64 &d, QObject *p = NULL )
                : Data64( d.v, d.typeCode, p ? p : d.parent() ) {}
              DataVbc64( const DataItemBA &di, QObject *p = NULL );
  DataItemBA  toDataItem( bool cf = false ) const;
virtual void  operator = ( const DataItemBA &di );
        void  operator = ( const qint64 &d ) { v = d; }
};

#endif // DATAVBC64_H
