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
#ifndef GENESISREF_H
#define GENESISREF_H

#include "genesisblock.h"

/**
 * @brief The GenesisRef class - identifies a chain
 */
class GenesisRef : public DataVarLength
{
    Q_OBJECT
public:
      explicit  GenesisRef( QByteArray di = QByteArray(), QObject *p = NULL );
                GenesisRef( const GenesisRef &r, QObject *p = NULL )
                  : DataVarLength( QByteArray(), AO_GENESIS_REF, p ? p : r.parent() ),
                    hash( r.hash ) {}
          void  operator = ( const QByteArray &di );
          Hash  getHash()    const { return  hash; }
          void  setHash( const Hash &h ) { hash = h; }
    QByteArray  toDataItem( bool cf = false );
          bool  isValid() { return hash.isValid(); }
      DataItem *getProp( const typeCode_t &key ) { return ( properties.contains( key ) ) ? properties.value(key) : new DataItem(AO_UNDEFINED_DATAITEM,this); }

private:
           Hash  hash;         // hash signature (unique ID) of the genesis block
    PropertyMap  properties;  // Collection of properties that describe the chain
};

#endif // GENESISREF_H
