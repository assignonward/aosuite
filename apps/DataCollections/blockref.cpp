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
BlockRef::BlockRef( DataItemBA di, QObject *p )
  : DataVarLength( AO_BLOCK_REF, p )
{ // See if there's anything interesting in the data item
  if ( di.size() > 0 )
    { if ( typeCodeOf( di ) != AO_BLOCK_REF )
        { // TODO: log an error
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
                { switch ( typeCodeOf( items ) ) // read valid items from the byte array, in any order
                    { case AO_TIME_RECORDED:
                        propTime = items;
                        break;

                      case AO_HASH256:
                      case AO_HASH512:
                        blkHash = items;
                        break;

                      case AO_GENESIS_REF:
                        genesis = items;
                        break;

                      case AO_SHARES_OUT:
                        shOut = items;
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

/**
 * @brief BlockRef::operator =
 * @param di - data item to assign
 */
void BlockRef::operator = ( const DataItemBA &di )
{ BlockRef temp( di );
  propTime = temp.propTime;
  blkHash  = temp.blkHash;
  shOut    = temp.shOut;
  genesis  = temp.genesis;
  typeCode = temp.typeCode;
  return;
}

/**
 * @brief BlockRef::toDataItem
 * @param cf - compact (or chain) form?  Pass along to children.
 * @return data item with the BlockRef contents
 */
DataItemBA  BlockRef::toDataItem( bool cf )
{ QByteArrayList dil;
  dil.append( propTime.toDataItem(cf) );
  if ( blkHash.isValid() )
    dil.append( blkHash.toDataItem(cf) );
  if ( shOut > 0 )
    dil.append( shOut.toDataItem(cf) );
  if ( genesis.isValid() )
    dil.append( genesis.toDataItem(cf) );
  std::sort( dil.begin(), dil.end() );
  ba = dil.join();
  return DataVarLength::toDataItem(cf);
}
