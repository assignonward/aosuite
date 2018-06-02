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
GenericCollection::GenericCollection( QByteArray di, QObject *p )
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
          if ( temp.checksumValidated() )
            { QByteArray items = temp.get();  // typeCode and checksum have been stripped off
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
}

/**
 * @brief GenericCollection::operator =
 * @param di - data item to assign
 */
void GenericCollection::operator = ( const QByteArray &di )
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
QByteArray  GenericCollection::toDataItem( bool cf )
{ QByteArrayList dil;
  QList<typeCode_t>keys = properties.keys();
  foreach ( typeCode_t key, keys )
    { dil.append( properties.value(key)->toDataItem(cf) );
      // qDebug( "appending key %x", key );
    }
  // TODO: randomize order of dil
  ba = dil.join();
  return DataVarLength::toDataItem(cf);
}

QByteArray  GenericCollection::toHashData( bool cf )
{ QByteArrayList dil;
  QList<typeCode_t>keys = properties.keys();
  foreach ( typeCode_t key, keys )
    dil.append( properties.value(key)->toHashData(cf) );
  // TODO: randomize order of dil
  ba = dil.join();
  return DataVarLength::toDataItem(cf);
}

