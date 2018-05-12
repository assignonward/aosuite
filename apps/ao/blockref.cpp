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
#include "blockref.h"

/**
 * @brief BlockRef::BlockRef - constructor
 * @param di - optional data item
 * @param p - optional parent object
 */
BlockRef::BlockRef( QByteArray di, QObject *p )
  : DataVarLenLong( AO_BLOCK_REF, QByteArray(), p )
{ // See if there's anything interesting in the data item
  if ( di.size() > 0 )
    { char tc = di.at(0);
      if ( reinterpret_cast<typeCode_t &>( tc ) != AO_BLOCK_REF )
        { // TODO: log an error
          return;
        }
       else
        { DataVarLenLong temp( di );          // It's our type
          if ( temp.checksumValidated() )
            { QByteArray items = temp.get();  // typeCode and checksum have been stripped off
              while ( items.size() > 0 )
                { int sz = typeSize( items );
                  if ( sz > 0 )
                    { switch ( items.at(0) ) // read valid items from the byte array, in any order
                        { case AO_TIME_RECORDED:
                            propTime = items;
                            break;

                          case AO_HASH256:
                          case AO_HASH512:
                            blkHash = items;
                            break;

                          default:
                            // TODO: log anomaly - unrecognized data type
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
 * @brief BlockRef::toDataItem
 * @return data item with the BlockRef contents
 */
QByteArray  BlockRef::toDataItem()
{ QList<QByteArray> dil;
  dil.append( propTime.toDataItem() );
  dil.append(  blkHash.toDataItem() );
  // TODO: randomize order of dil
  ba.clear();
  foreach( QByteArray a, dil )
    ba.append( a );
  return DataVarLenLong::toDataItem();
}
