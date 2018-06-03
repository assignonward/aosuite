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
#include "genericcollection.h"

/**
 * @brief GenericCollection::GenericCollection - constructor
 * @param di - optional data item
 * @param tc - type code
 * @param p - optional parent object
 */
GenericCollection::GenericCollection( DataItemBA di, QObject *p )
                     : DataVarLength( typeCodeOf(di), p )
{ // See if there's anything interesting in the data item
  if ( di.size() > 0 )
    { if (( typeCodeOf( di ) != GB_GENESIS_BLOCK ) &&
          ( typeCodeOf( di ) != CB_CHAIN_BLOCK   )) // add more as they are defined
        { typeCode = AO_UNDEFINED_DATAITEM;
          // TODO: log an error
          return;
        }
       else
        { DataVarLength temp( di );          // It's our type
          DataItemBA items = temp.get();  // typeCode and checksum have been stripped off
          while ( items.size() > 0 )
            { int sz = typeSize( items );
              if ( sz <= 0 )
                { // TODO: log error
                  return;
                }
               else
                { properties.insert( typeCodeOf( items ), DataItem::fromDataItem( items, this ) );
                  items = items.mid( sz ); // move on to the next
                }
            }

        }
    }
}

/**
 * @brief GenericCollection::operator =
 * @param di - data item to assign
 */
void GenericCollection::operator = ( const DataItemBA &di )
{ GenericCollection temp( di );
  properties = temp.properties;
  typeCode   = temp.typeCode;
  return;
}

/**
 * @brief GenericCollection::toDataItem
 * @param cf - compact (or chain) form?  Pass along to children.
 * @return data item with the BlockRef contents
 */
DataItemBA  GenericCollection::toDataItem( bool cf ) const
{ // qDebug( "GenericCollection::toDataItem" );
  QByteArrayList dil;
  QList<typeCode_t>keys = properties.keys();
  foreach ( typeCode_t key, keys )
    { dil.append( properties.value(key)->toDataItem(cf) );
      // qDebug( "appending key %x", key );
    }
  std::sort( dil.begin(), dil.end() );
  QByteArray dba = dil.join();
  DataItemBA di;
  di.append( codeToBytes( typeCode   ) );
  di.append( codeToBytes( dba.size() ) ); // variable length form...
  di.append( dba );
  return di;
}

DataItemBA  GenericCollection::toHashData( bool cf ) const
{ // qDebug( "GenericCollection::toHashData" );
  QList<DataItemBA> dil;
  QList<typeCode_t>keys = properties.keys();
  foreach ( typeCode_t key, keys )
    dil.append( properties.value(key)->toDataItem(cf) );
  std::sort( dil.begin(), dil.end() );
  QByteArrayList hdl;
  foreach ( QByteArray di, dil )
    { if ( typeCodeOf( di ) & AO_SEPARABLE_TYPE )
        hdl.append( properties.value(typeCodeOf( di ))->toHashData(cf) );
       else
        hdl.append( di );
    }
  QByteArray dba = hdl.join();
  QByteArray hd;
  hd.append( codeToBytes( typeCode   ) );
  hd.append( codeToBytes( dba.size() ) ); // variable length form...
  hd.append( dba );
  // qDebug( "  hdat:%s",qPrintable( QString::fromUtf8( hd.toHex() ) ) );
  return hd;
}

