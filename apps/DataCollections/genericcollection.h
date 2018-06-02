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
#ifndef GENERICCOLLECTION_H
#define GENERICCOLLECTION_H

#include "datavarlength.h"
#include "hash.h"
#include <QMap>

#define DataItemMap QMap<typeCode_t,DataItem *>

/**
 * @brief The GenericCollection class - the anchorpoint of a blockchain
 */
class GenericCollection : public DataVarLength
{
    Q_OBJECT
public:
      explicit  GenericCollection( QByteArray di = QByteArray(), QObject *p = NULL );
                GenericCollection( typeCode_t tc, QObject *p = NULL )
                  : DataVarLength( QByteArray(), tc, p ) {}
                GenericCollection( const GenericCollection &r, QObject *p = NULL )
                  : DataVarLength( QByteArray(), r.typeCode, p ? p : r.parent() ),
                    properties( r.properties ) {}
          void  operator = ( const QByteArray &di );
    QByteArray  toDataItem( bool cf = false );
    QByteArray  toHashData( bool cf = false );
      DataItem *getProp( typeCode_t key ) { return ( properties.contains( key ) ) ? properties.value( key ) : NULL; }
          void  add( const typeCode_t& key, DataItem *value ) { properties.insert( key, value ); }
private:
    DataItemMap  properties;  // Collection of properties that describe the chain
};

#endif // GENERICCOLLECTION_H
