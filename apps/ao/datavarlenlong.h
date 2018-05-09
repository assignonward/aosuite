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
#ifndef DATAVARLENLONG_H
#define DATAVARLENLONG_H

#include <QObject>
#include "bytecodes.h"

class DataVarLenLong : public QObject
{
    Q_OBJECT
public:
    explicit DataVarLenLong( unsigned char t = AO_VARLONG_INVALID, QByteArray iba = QByteArray(), QObject *p = nullptr )  : QObject( p ), ba( iba ), typeCode( t ) {}
             DataVarLenLong( const DataVarLenLong &d, QObject *p = nullptr ) : QObject( p ? p : d.parent() ), ba( d.ba ), typeCode( d.typeCode ) {}
             DataVarLenLong( const QByteArray &di, QObject *p = nullptr );
  QByteArray toDataItem();
        void operator =  ( const QByteArray &di );
        void operator =  ( const DataVarLenLong &d ) { ba = d.ba; typeCode = d.typeCode; }
        bool operator == ( const DataVarLenLong &d ) { return ba == d.ba; }
        bool operator != ( const DataVarLenLong &d ) { return ba != d.ba; }
  QByteArray get() { return ba; } // Just the meat, without typecode or checksum
        void set( QByteArray sba );

protected:
     QByteArray ba;       // generic data, not including type or checksum
  unsigned char typeCode; // what kind of var length data is this?
};

#endif // DATAVARLENLONG_H
