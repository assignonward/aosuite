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
#ifndef DATAVARLENSHORT_H
#define DATAVARLENSHORT_H

#include "dataitem.h"

class DataVarLenShort : public DataItem
{
    Q_OBJECT
public:
    explicit  DataVarLenShort( typeCode_t tc = AO_UNDEFINED_DATAITEM, QByteArray iba = QByteArray(), QObject *p = nullptr )
                : DataItem( tc, p ), ba( iba ) {}
              DataVarLenShort( typeCode_t tc = AO_UNDEFINED_DATAITEM, QObject *p = nullptr )
                : DataItem( tc, p ) {}
              DataVarLenShort( const DataVarLenShort &d, QObject *p = nullptr )
                : DataItem( d.typeCode, p ? p : d.parent() ), ba( d.ba ) {}
              DataVarLenShort( const QByteArray &di, QObject *p = nullptr );
  virtual QByteArray toDataItem() const;
        void  operator =  ( const QByteArray &di );
        void  operator =  ( const DataVarLenShort &d ) { ba = d.ba; typeCode = d.typeCode; csVal = d.csVal; }
        bool  operator == ( const DataVarLenShort &d ) { return ba == d.ba; }
        bool  operator != ( const DataVarLenShort &d ) { return ba != d.ba; }
  QByteArray  get() const { return ba; } // Just the meat, without typecode or checksum
        void  set( QByteArray sba ) { ba = sba; }

protected:
  QByteArray  ba;    // generic data, not including type or checksum
};

#endif // DATAVARLENSHORT_H