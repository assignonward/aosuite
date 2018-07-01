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
#ifndef DATAITEM_H
#define DATAITEM_H

#define INCLUDE_TESTS

#include <QObject>
#include "dataitem-auto.h"
#include "dataitemba.h"
#include "varsizecode.h"

#define typeCode_t qint64
#define AO_SEPARABLE_TYPE1         0x1000 // Types with these bits unequale are separable
#define AO_SEPARABLE_TYPE2         0x0400 // Types with these bits unequale are separable

class DataItem : public QObject, public VarSizeCode
{
    Q_OBJECT
public:
           explicit  DataItem( typeCode_t tc = AO_UNDEFINED_DATAITEM, QObject *p = NULL )
                       : QObject(p), typeCode( tc ) {}
                     DataItem( const DataItem &i, QObject *p = NULL )
                       : QObject(p ? p : i.parent()), typeCode( i.typeCode ) {}
               void  operator = ( const DataItem &i )
                       { typeCode = i.typeCode; }
       virtual void  operator = ( const DataItemBA &di ) { typeCode = typeCodeOf(di); }
             qint32  typeSize( typeCode_t tc = AO_UNDEFINED_DATAITEM ) const;
             qint32  typeSize( const DataItemBA &di ) const;
  static   DataItem *fromDataItem( const DataItemBA &di, QObject *p = NULL );
  static   DataItem *fromDataItem( const DataItem   *di, QObject *p = NULL );
  static     qint32  typeSizeTable( typeCode_t tc );
  static typeCode_t  typeCodeOf( const DataItemBA &di );
  static       bool  typeCodeIsSeparable( typeCode_t tc );
               bool  typeCodeIsSeparable() const;
 virtual DataItemBA  toDataItem( bool cf = false ) const;
 virtual DataItemBA  toHashData( bool cf = false ) const;
 virtual       void  debugShow( qint32 level = 0 ) const;
         DataItemBA  getHash( typeCode_t ht = AO_HASH256 ) const;
               bool  verifyHash( const DataItemBA &hdi ) const;
         typeCode_t  getTypeCode() const { return typeCode; }
               void  setTypeCode( const typeCode_t &tc ) { typeCode = tc; }

// Testing code
#ifdef INCLUDE_TESTS
               bool  testHashVerify();
               bool  testHashVerifyType( typeCode_t ht );
#endif

protected:
  typeCode_t  typeCode; // what kind of data item is this?
};

#endif // DATAITEM_H
