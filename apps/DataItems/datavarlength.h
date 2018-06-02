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
#ifndef DATAVARLENGTH_H
#define DATAVARLENGTH_H

#include "dataitem.h"

class DataVarLength : public DataItem
{
    Q_OBJECT
public:
          explicit  DataVarLength( typeCode_t tc = AO_UNDEFINED_DATAITEM, QObject *p = NULL )
                      : DataItem( tc, p ) {}
                    DataVarLength( QByteArray iba, typeCode_t tc, QObject *p = NULL )
                      : DataItem( tc, p ), ba( iba ) {}
                    DataVarLength( const DataVarLength &d, QObject *p = NULL )
                      : DataItem( d.typeCode, p ? p : d.parent() ), ba( d.ba ) {}
                    DataVarLength( const DataItemBA &di, QObject *p = NULL );
virtual DataItemBA  toDataItem( bool cf = false ) const;
      virtual void  operator =  ( const DataItemBA &di );
              void  operator =  ( const DataVarLength &d ) { ba = d.ba; typeCode = d.typeCode; csVal = d.csVal; }
              bool  operator == ( const DataVarLength &d ) { return ba == d.ba; }
              bool  operator != ( const DataVarLength &d ) { return ba != d.ba; }
        QByteArray  get() const { return ba; } // Just the meat, without typecode or checksum
              void  set( QByteArray sba ) { ba = sba; }
            qint32  size() const { return ba.size(); }

protected:
        QByteArray  ba;    // generic data, not including type or checksum
};

#endif // DATAVARLENGTH_H
