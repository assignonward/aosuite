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
#include "genesisblock.h"

/**
 * @brief GenesisBlock::GenesisBlock - constructor
 * @param di - optional data item
 * @param p - optional parent object
 */
GenesisBlock::GenesisBlock( QByteArray di, QObject *p )
                : DataVarLength( AO_GENESIS_BLOCK, p )
{ // See if there's anything interesting in the data item
  if ( di.size() > 0 )
    { if ( typeCodeOf( di ) != AO_GENESIS_BLOCK )
        { // TODO: log an error
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
                    { switch ( typeCodeOf( items ) ) // read valid items from the byte array, in any order
                        { case AO_HASH256:
                          case AO_HASH512:
                            hash = items;
                            break;

                          default:
                            properties.insert( typeCodeOf( items ), DataItem::fromDataItem( items, this ) );
                            break;
                        }
                      items = items.mid( sz ); // move on to the next
                    }
                }
            }
        }
    }
}

/**
 * @brief GenesisBlock::operator =
 * @param di - data item to assign
 */
void GenesisBlock::operator = ( const QByteArray &di )
{ GenesisBlock temp( di );
  hash       = temp.hash;
  properties = temp.properties;
  typeCode   = temp.typeCode;
  return;
}

/**
 * @brief GenesisBlock::toDataItem
 * @param cf - compact (or chain) form?  Pass along to children.
 * @return data item with the BlockRef contents
 */
QByteArray  GenesisBlock::toDataItem( bool cf )
{ QByteArrayList dil;
  if ( hash.isValid() )
    dil.append( hash.toDataItem(cf) );
  QList<typeCode_t>keys = properties.keys();
  foreach ( typeCode_t key, keys )
    dil.append( properties.value(key)->toDataItem(cf) );
  // TODO: randomize order of dil
  ba = dil.join();
  return DataVarLength::toDataItem(cf);
}
