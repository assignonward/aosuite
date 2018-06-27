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
// DataFixedLength is the base class for objects which are byte arrays of fixed length.
//
#ifndef DATAFIXEDLENGTH_H
#define DATAFIXEDLENGTH_H

#include "dataitem.h"

class DataFixedLength : public DataItem
{
    Q_OBJECT
public:
    explicit  DataFixedLength( typeCode_t tc = AO_UNDEFINED_DATAITEM, QByteArray iba = QByteArray(), QObject *p = NULL )
                : DataItem( tc, p ), ba( iba ) {}
              DataFixedLength( typeCode_t tc = AO_UNDEFINED_DATAITEM, QObject *p = NULL )
                : DataItem( tc, p ) {}
              DataFixedLength( const DataFixedLength &d, QObject *p = NULL )
                : DataItem( d.typeCode, p ? p : d.parent() ), ba( d.ba ) {}
              DataFixedLength( const DataItemBA &di, QObject *p = NULL );
  DataItemBA  toDataItem( bool cf = false ) const;
virtual void  operator =  ( const DataItemBA &di );
        void  operator =  ( const DataFixedLength &d ) { ba = d.ba; typeCode = d.typeCode; }
        bool  operator == ( const DataFixedLength &d ) { return ba == d.ba; }
        bool  operator != ( const DataFixedLength &d ) { return ba != d.ba; }
  QByteArray  get() const { return ba; } // Just the meat, without typecode or checksum
        void  set( QByteArray sba );
// virtual bool  isValid() const { return ( ( typeSize() - 2 ) == ba.size() ); }
virtual bool  isValid() const { return true; }
        void  debugShow( qint32 level = 0 ) const;

protected:
  QByteArray  ba;       // generic data, not including type or checksum
};


#endif // DATAFIXEDLENGTH_H
